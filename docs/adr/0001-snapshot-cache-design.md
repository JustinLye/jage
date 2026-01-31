# ADR-0001: Snapshot Cache Design

**Status:** Accepted (Implementation Pending)
**Date:** 2026-01-31
**Authors:** Justin Lye, Claude (Sonnet 4.5)

---

## Context

### Problem Statement

Game systems generate timestamped events (input events, physics collisions, network messages, diagnostics) that must be correlated with game state at the time they occurred. Time snapshots are taken at the start of each frame and define temporal windows for event correlation.

**Example Timeline:**

| Timestamp (ns)      | Event                          | Frame Index |
|---------------------|--------------------------------|-------------|
| 1769834083000000100 | Frame N begins                 | N           |
| 1769834083000000112 | **Snapshot[N] taken**          | N           |
| 1769834083000000113 | Key press event occurs         | N           |
| 1769834083000000130 | Input adapter polled           | N           |
| 1769834083000000220 | **Cache queried for event**    | N           |

**Query:** `find(timestamp=1769834083000000113)`
**Expected:** Snapshot[N] (timestamp 112), `match_status::matched`

**Why:** Event at T+113 occurred during frame N's window, which started at T+100 and was "closed" by Snapshot[N] at T+112.

### Frame Window Semantics

- **Snapshot[N]** taken at start of frame N
- **Events between Snapshot[N-1] and Snapshot[N]** belong to frame N-1
- **Events between Snapshot[N] and Snapshot[N+1]** belong to frame N

### Requirements

**Functional:**
1. Find snapshot by real-time timestamp (nanoseconds)
2. Find snapshot by frame index (monotonic frame counter)
3. Indicate match status: exact match, ahead of cache, or evicted
4. Support continuous operation for application lifetime (minutes to hours)

**Non-Functional:**
1. **Performance:** Low latency lookup (<2 μs), lock-free SPMC
2. **Memory:** Fixed size, automatic eviction, cache-efficient
3. **Thread Safety:** One writer (clock), multiple readers (input handlers, diagnostics, replay)

---

## Decision

### Core Architecture

Implement a **private sliding window buffer within `snapshot_cache`** with:

- Fixed-capacity circular buffer (default: 120 snapshots = 2 seconds at 60 FPS)
- Lock-free SPMC (single producer, multiple consumers)
- **Double buffer per slot** for torn-read protection
- `match_status` enum for query results
- Private implementation (not extracted as general-purpose container)

```cpp
namespace jage::time {

enum class match_status : std::uint8_t {
    matched,
    ahead,
    evicted
};

template<std::size_t Capacity = 120>
class snapshot_cache {
private:
    using slot_type = concurrency::double_buffer<events::snapshot>;
    alignas(memory::cacheline_size) std::array<slot_type, Capacity> buffer_{};
    alignas(memory::cacheline_size) std::atomic<std::size_t> write_index_{0};

public:
    auto push(events::snapshot snapshot) -> void;

    template<concepts::duration Duration>
    [[nodiscard]] auto find(const Duration& timestamp) const
        -> std::pair<events::snapshot, match_status>;

    [[nodiscard]] auto find(std::uint64_t frame_index) const
        -> std::pair<events::snapshot, match_status>;
};

} // namespace jage::time
```

---

## Alternatives Considered

### 1. General-Purpose Container vs Private Implementation

**Alternative A: Extract `jage::containers::sliding_window<T, Capacity>`**

Pros:
- Reusable for frame timing, input sampling, audio buffers
- Clear separation of concerns

Cons:
- Premature abstraction (only one use case currently)
- Would pay for unused generality (size tracking, partial fill support)

**Alternative B: Extend `jage::containers::queue`**

Pros:
- Code reuse

Cons:
- Wrong abstraction (queue is FIFO consumption, cache is temporal lookup)
- Queue has SPSC semantics, cache needs SPMC

**Chosen: Private implementation**

Rationale:
- Highly specialized requirements (always full, backward search, domain-specific results)
- "Rule of three": Wait for 2-3 use cases before extracting abstraction
- Easier to evolve as requirements emerge

Assumptions:
- Future use cases might have different needs
- When extracted, interface will be clearer with real examples

---

### 2. Search Algorithm

**Alternative A: Two separate loops**

```cpp
// Loop 1: [oldest, 0)
for (auto i = oldest; i > 0; --i) { check buffer_[i-1]; }

// Loop 2: [Capacity-1, oldest]
for (auto i = Capacity - 1; i >= oldest; --i) { check buffer_[i]; }
```

Pros:
- Avoids modulo (~2-10 cycles per iteration)

Cons:
- Two loops with different index logic (error-prone)
- Harder to verify correctness
- Implicit wrap-around

**Chosen: Single loop with modulo**

```cpp
for (std::size_t offset = 0; offset < Capacity; ++offset) {
    auto index = (newest_index - offset + Capacity) % Capacity;
    // Search...
}
```

Rationale:
- Clear intent: "search backward, wrapping automatically"
- Easy to verify: "iterate exactly Capacity times"
- Modulo overhead negligible (~240-1200 cycles for 120 iterations = <0.01% of frame budget)
- Modern compilers optimize `% Capacity` to bitwise AND for power-of-2

Performance:
- Best case: ~10 ns (most recent snapshot)
- Average: ~500 ns - 1 μs (60 iterations)
- Worst: ~1-2 μs (120 iterations)
- Context: At 60 FPS, one frame = 16,667 μs

Future optimization: Binary search (O(log 120) = 7 iterations) if profiling shows need.

---

### 3. Match Status Representation

**Alternative A: `accuracy` enum**

```cpp
enum class accuracy { exact, estimate };
```

Cons: Doesn't distinguish why estimate was returned (ahead vs evicted)

**Alternative B: `snapshot_availability`**

```cpp
enum class snapshot_availability { available, not_yet_captured, evicted };
```

Cons: Verbose, focuses on cache state rather than query outcome

**Chosen: `match_status`**

```cpp
enum class match_status : std::uint8_t {
    matched, ahead, evicted
};
```

Rationale:
- Action-oriented (describes query outcome)
- Clear semantics for each case
- Extensible (can add `exact` vs `interpolated` later)

---

### 4. Synchronization Strategy (CRITICAL)

**Alternative A: Mutex per slot or global lock**

Cons:
- Lock contention
- Not lock-free
- Priority inversion

**Alternative B: Sequence lock (seqlock)**

```cpp
std::atomic<uint64_t> seq_version_;
// Writer: ++seq, write, ++seq
// Reader: retry if seq odd or changed
```

Pros:
- Detects torn reads, retries
- Lock-free

Cons:
- More complex
- Retry overhead
- Not guaranteed progress (live-lock possible)

**Alternative C: Atomic index + acquire/release (original proposal)**

```cpp
// Writer:
write_index_.store(index + 1, std::memory_order::release);

// Reader:
auto snapshot = buffer_[index];  // Copy to local
```

Pros:
- Simple, minimal overhead
- Lock-free

Cons:
- **CRITICAL FLAW:** Torn reads possible!
- Reader might copy buffer_[index] while writer overwrites it
- Result: Corrupted snapshot (half old data, half new data)

**Chosen: Double buffer per slot**

```cpp
std::array<concurrency::double_buffer<events::snapshot>, Capacity> buffer_;

// Writer:
buffer_[index].write(snapshot);

// Reader:
auto snapshot = buffer_[index].read();
```

Pros:
- **Guaranteed consistent reads** (no torn data)
- `double_buffer` already implemented and tested
- Cacheline-aligned (prevents false sharing)
- Simple interface

Cons:
- 2x memory per slot (~15 KB overhead for 120 capacity)

Rationale:
- **Correctness over optimization:** Torn reads are NEVER acceptable
- Memory cost negligible (15 KB = 0.00009% of 16 GB RAM)
- Performance benefit: Cacheline alignment prevents false sharing
- Reuses battle-tested `double_buffer` implementation

### Torn Read Example

**Without double buffer:**

```
T0: Reader starts copying buffer_[0] (64 bytes)
    Reads: ticks=1, real_time=100.0

T1: Writer overwrites buffer_[0] with frame 5
    Writes: ticks=5, real_time=200.0, scaled_time=200.0

T2: Reader finishes copy
    Gets: ticks=1, real_time=100.0, scaled_time=200.0  ← INCONSISTENT!
```

**With double buffer:**

```
T0: Reader calls buffer_[0].read()
    Atomically reads consistent snapshot from slot A

T1: Writer calls buffer_[0].write(new_snapshot)
    Writes to slot B, flips index

T2: Next reader reads from slot B (new data)
    Previous reader still has consistent slot A data
```

### Memory Footprint Analysis

| Configuration | Per Slot | Total (120) | % of 16GB RAM |
|--------------|----------|-------------|---------------|
| Raw array | 64 bytes | 7.5 KB | 0.000046% |
| Double buffer | 192 bytes | 22.5 KB | 0.00014% |
| **Overhead** | **+128 bytes** | **+15 KB** | **+0.00009%** |

Context:
- Single 512x512 RGBA texture: 1 MB (67x larger)
- 1 second of audio (48kHz stereo): 200 KB (13x larger)
- Typical mesh vertex buffer: 100 KB - 10 MB

**Conclusion:** 15 KB is FREE on modern hardware.

---

### 5. Capacity and Size Tracking

**Alternative: Track current size**

```cpp
auto size() const -> std::size_t {
    auto head = write_index_.load();
    return std::min(head, Capacity);
}
```

Cons: Extra check on every query for 0.33% of runtime (cold start)

**Chosen: Assume always full**

Rationale:
- After ~2 seconds, cache is always full (99.67% of runtime)
- Optimize for steady state, not cold start
- Simpler logic (no partial-fill checks)

Assumptions:
- Applications run >2 seconds (if not, snapshot accuracy doesn't matter)
- Warm-up handled via documented contract or pre-filling

---

### 6. Out-of-Range Behavior

**Alternative A: Return `std::nullopt`**

Cons: Caller must check `has_value()` even for common case

**Alternative B: Throw exception**

Cons: Exceptions in hot path, not zero-cost

**Chosen: Return boundary + status**

```cpp
if (timestamp > newest) return {newest, match_status::ahead};
if (timestamp < oldest) return {oldest, match_status::evicted};
```

Rationale:
- Graceful degradation (always returns usable snapshot)
- Zero overhead (no exceptions, no optionals)
- Caller decides how to handle via `match_status`

---

## Consequences

### Positive

1. **Correctness:** Double buffer eliminates torn reads completely
2. **Performance:** Lock-free, <2 μs queries, negligible frame impact
3. **Simplicity:** Reuses tested `double_buffer`, clear search algorithm
4. **Memory efficiency:** 22.5 KB total (negligible on modern systems)
5. **Extensibility:** Can add binary search, interpolation, diagnostics later
6. **Thread safety:** SPMC-safe with zero contention

### Negative

1. **Memory overhead:** 3x raw array (but still negligible)
2. **Cold start:** First 2 seconds may return default snapshots (if not pre-filled)
3. **Coupling:** Private implementation means no immediate reuse for other subsystems

### Neutral

1. **Linear search:** Acceptable for now, can optimize to binary if needed
2. **Fixed capacity:** Requires tuning per use case (default 120 = 2 seconds at 60 FPS)

---

## Implementation Notes

### Memory Layout

```
┌─────────────────────────────────────┐
│ buffer_[0]: double_buffer           │ ← Cacheline aligned
│   - slot_A: snapshot (64 bytes)     │
│   - slot_B: snapshot (64 bytes)     │
│   - index: atomic (8 bytes + pad)   │
├─────────────────────────────────────┤
│ buffer_[1]: double_buffer           │
│ ...                                 │
│ buffer_[119]: double_buffer         │
├─────────────────────────────────────┤ ← Cacheline boundary
│ write_index_: atomic                │ ← Separate cacheline
└─────────────────────────────────────┘
```

**Rationale:** Prevent false sharing between producer and consumers.

### Warm-Up Strategy

**Recommended: Document contract**

```cpp
// User responsibility: Push Capacity snapshots before querying
for (int i = 0; i < 120; ++i) {
    cache.push(clock.tick());
}
```

**Alternative: Pre-fill constructor**

```cpp
snapshot_cache() {
    for (auto& slot : buffer_) {
        slot.write(events::snapshot{});  // Default snapshot
    }
    write_index_.store(Capacity);
}
```

Current choice: Document contract (simpler, no garbage data).

### `std::unreachable()` Usage

```cpp
// After boundary checks, search MUST find match
#ifdef NDEBUG
    std::unreachable();  // Optimize
#else
    assert(false && "Timestamp must be in range");
    return {newest, match_status::matched};  // Safe fallback
#endif
```

**Rationale:**
- Debug: Crashes with message if logic bug
- Release: Compiler optimizes assuming unreachable
- UBSan can detect if reached during testing

---

## Testing Strategy

### Unit Tests

1. **Functional:**
   - Push snapshots, verify FIFO eviction
   - Find by timestamp (exact match)
   - Find by frame index
   - Boundary cases (ahead, evicted)

2. **Concurrency (CRITICAL):**
   ```cpp
   "should never return torn snapshot under concurrent access"_test = [] {
       snapshot_cache<120> cache;
       std::atomic<bool> running{true};

       // Writer: continuously push
       auto writer = std::jthread([&] {
           while (running) cache.push(make_snapshot(frame++));
       });

       // Readers: continuously query and verify consistency
       auto readers = std::vector<std::jthread>{};
       for (int i = 0; i < 4; ++i) {
           readers.emplace_back([&] {
               while (running) {
                   auto [snap, status] = cache.find(some_timestamp);
                   expect(is_consistent(snap));  // Never torn!
               }
           });
       }

       std::this_thread::sleep_for(1s);
       running = false;
   };
   ```

3. **Edge cases:**
   - Wrap-around (oldest index > newest index in buffer)
   - Cold start (query before Capacity snapshots pushed)
   - Timestamp exactly matching snapshot

### Performance Tests (Future)

- Microbenchmark: Queries/second under contention
- Profile: Linear vs binary search
- Memory: Cacheline alignment validation (no false sharing)

---

## Future Considerations

### When to Extract General Container

Extract `jage::containers::sliding_window<T, Capacity>` when:
1. Second use case emerges (frame timing, input sampling, audio buffer)
2. Interface requirements clear from multiple examples
3. Commonalities outweigh differences

### Optimizations (Data-Driven)

**Binary search for timestamp lookup:**
```cpp
// O(log 120) = 7 iterations vs O(60) average
// Only if profiling shows linear search >5% frame time
```

**Diagnostics to guide decisions:**
- Cache hit rate (`matched` vs `ahead`/`evicted`)
- Query timestamp age distribution
- Average search depth
- False sharing metrics (cache misses)

**Interpolation between snapshots:**
```cpp
match_status::interpolated  // Event between two snapshots
```

### Platform Considerations

**Double buffer assumptions:**
- Cacheline size = 64 bytes (x86-64, ARM64)
- If targeting exotic platforms, use runtime detection

**Atomic operations:**
- `double_buffer` uses acquire/release semantics
- Portable across x86-64, ARM64, RISC-V

---

## References

- **Milestone:** [Snapshot Cache #2](https://github.com/JustinLye/jage/milestone/2)
- **Brainstorming:** `docs/planning/time/snapshot-cache/brainstorming.md`
- **GitHub Issue:** [Two-loop vs modulo comparison #51](https://github.com/JustinLye/jage/issues/51)
- **Double Buffer:** `include/jage/concurrency/double_buffer.hpp`
- **Memory Ordering:** [cppreference: memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)
- **Lock-Free Programming:** Herlihy & Shavit, "The Art of Multiprocessor Programming"

---

## Revision History

| Date       | Author       | Changes                                      |
|------------|--------------|----------------------------------------------|
| 2026-01-31 | Justin Lye   | Initial ADR, chose double_buffer per slot    |
