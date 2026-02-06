# ADR-0004: Ring Buffer

**Status:** Accepted
**Date:** 2026-02-04
**Authors:** Justin Lye

---

## Context

### Problem Statement

The input event bus delivers timestamped input events from a single producer (the bus) to multiple consumers (game systems: UI, Gameplay, Diagnostics, Recording, Simulation). Each consumer processes events at its own pace. The data structure must:

1. Support one writer, N readers concurrently
2. Never back-pressure the writer — if a reader falls behind, that reader's problem, not the system's
3. Allow each reader to track its own position independently
4. Guarantee consistent reads under concurrent writes (no torn data)

### Requirements

**Functional:**
1. Single producer publishes events into a fixed-capacity circular buffer
2. Multiple consumers read from the buffer at independent rates
3. Each consumer tracks its own read position via a monotonic index
4. Consumers detect when they have been lapped (overflow) via index comparison
5. Reads are guaranteed consistent regardless of concurrent writes

**Non-Functional:**
1. **Lock-free:** No mutexes on the hot path (publish or read)
2. **Zero allocation:** Fixed-capacity buffer, no heap in hot path
3. **Cache-friendly:** Minimize false sharing between writer and readers
4. **Reuse:** Leverage existing `double_buffer` for torn-read protection

### Design Goals

| Goal | Implication |
|------|-------------|
| No back-pressure on writer | Writer never blocks or slows for slow readers |
| Isolated overflow | A slow reader loses its own events, not others' |
| Lock-free | Atomic operations only, no mutexes |
| Cacheline awareness | `write_head` on its own cacheline; slots cacheline-aligned |
| Reuse tested primitives | `double_buffer` per slot — already implemented and tested |

---

## Decision

### Architecture

```
                        write_head (atomic, release on publish)
                            ↓
  ┌──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┐
  │slot 0│slot 1│slot 2│slot 3│slot 4│slot 5│slot 6│slot 7│   ← Capacity slots
  │(db)  │(db)  │(db)  │(db)  │(db)  │(db)  │(db)  │(db)  │      each: double_buffer<TEvent>
  └──────┴──────┴──────┴──────┴──────┴──────┴──────┴──────┘
            ↑                   ↑              ↑
      reader A head       reader B head   reader C head
      (consumer-owned)    (consumer-owned)
```

- Writer publishes into `slots_[write_head % Capacity]`, then increments `write_head` with release semantics.
- Readers load `write_head` with acquire semantics, then read slots from their own head up to `write_head`.
- No reader state is visible to the ring or to other readers.

### Monotonic Index Design

Neither `write_head` nor consumer read heads wrap at capacity. They increment forever:

```
write_head:      0, 1, 2, ... 255, 256, 257, ...   (never resets)
Physical slot:   write_head % Capacity

Consumer detects lapped:
    write_head - read_head >= Capacity  →  lapped
```

**Why monotonic:**
- Distance between writer and reader is a simple subtraction — no modular arithmetic ambiguity
- Lapping detection is a single comparison
- Same pattern used by `snapshot_cache` (`write_index_`)

### Torn-Read Protection

Each slot is a `concurrency::double_buffer<TEvent>`. Writer writes into one sub-slot while readers read from the other. Reads are guaranteed consistent regardless of concurrent writes.

**Why `double_buffer`:**
- Already implemented and tested in `jage::concurrency`
- Guaranteed consistent reads — no retry loops, no seqlocks
- Cacheline-aligned per slot — prevents false sharing between adjacent slots
- Same strategy `snapshot_cache` uses — proven correct under concurrent access

### Interface

```cpp
namespace jage::containers::spmc {

template <typename TEvent, std::size_t Capacity>
class ring_buffer {
public:
    // Producer interface — called by input_event_bus
    void push(const TEvent& event);

    // Consumer interface — called by game systems
    [[nodiscard]] auto write_head() const -> std::size_t;
    [[nodiscard]] auto read(std::size_t index) const -> TEvent;

    [[nodiscard]] static constexpr auto capacity() -> std::size_t;

private:
    std::array<concurrency::double_buffer<TEvent>, Capacity> slots_{};
    alignas(memory::cacheline_size) std::atomic<std::size_t> write_head_{0};
};

} // namespace jage::containers::spmc
```

**CRITICAL: Caller Responsibility for Index Bounds**

The `read(index)` method does **not** apply modulo or bounds-check the index. The caller is responsible for ensuring `index < capacity()` by applying modulo before calling `read()`.

**Correct usage:**
```cpp
event = ring.read(monotonic_index % ring.capacity());  // ✓ Caller applies modulo
```

**Incorrect usage (undefined behavior):**
```cpp
event = ring.read(monotonic_index);  // ✗ UB if monotonic_index >= capacity
```

### Read Head Ownership

Read heads are **not** owned by the ring. They live on the consumer side:

```cpp
class ui_system {
    std::reference_wrapper<const ring_buffer<input_event, N>> ring_;
    std::size_t read_head_{0};   // plain size_t — only this thread touches it
};
```

**Why consumer-owned:**
- Ring is decoupled from consumers — no knowledge of who is reading or how many
- No `MaxSubscribers` template parameter or dynamic allocation
- Read head is touched by exactly one thread — no atomics needed
- Matches the independence-first design: each consumer is responsible for its own drain, lap detection, and missed-event policy

### Consumer Drain Pattern

```
// Pseudocode — lives in the game system, not in the ring

1. Load write_head from ring                     (acquire)
2. Compare write_head to my read_head

3a. If (write_head - read_head >= Capacity):
        → Lapped. Some events are gone.
        → Reset read_head to (write_head - Capacity)   // oldest surviving slot
        → Handle missed events (log, notify diagnostics, etc.)

3b. If (write_head - read_head < Capacity):
        → All pending events still in ring. Proceed normally.

4. Drain: for each slot from read_head to write_head:
        event = ring.read(slot % Capacity)             // double_buffer read
        process(event)

5. Update read_head = write_head                       // caught up
```

### Memory Layout

```
┌─────────────────────────────────────────┐
│ slots_[0]: double_buffer<TEvent>        │ ← cacheline aligned
│   sub_slot_A: TEvent                    │
│   sub_slot_B: TEvent                    │
│   active:     atomic (slot selector)    │
├─────────────────────────────────────────┤
│ slots_[1]: double_buffer<TEvent>        │
│ ...                                     │
│ slots_[Capacity-1]: double_buffer       │
├─────────────────────────────────────────┤ ← cacheline boundary
│ write_head_: atomic<size_t>             │ ← own cacheline, no false sharing with slots
└─────────────────────────────────────────┘
```

### Capacity Sizing

For the input event bus use case:

| Event | Frequency | Events/frame @ 60 FPS |
|-------|-----------|----------------------|
| Cursor | up to 1000 Hz | ~17 |
| Raw motion | up to 1000 Hz | ~17 |
| Keyboard | ~20/sec | <1 |
| Mouse buttons | ~10/sec | <1 |
| Scroll | burst | 0–5 |
| **Total** | | **~35–40** |

Recommended starting capacity: **256 slots** (~7 frames of headroom at peak input rate).

Total memory at 256 capacity with `double_buffer` per slot: **~48 KB**.

| Gaming PC Tier | Typical RAM | 48 KB as % of total |
|----------------|-------------|---------------------|
| Low-end | 8 GB | 0.00000057% |
| Mid-range | 16 GB | 0.00000029% |
| High-end | 32–64 GB | 0.00000014–0.00000007% |

---

## Alternatives Considered

### 1. Separate Ring Per Event Type

**Alternative:** One `ring_buffer` per event type (keyboard ring, cursor ring, mouse ring, etc.). Each consumer reads only the rings it needs.

**Rejected because:**

A consumer scanning a single ring for one event type walks past events it does not care about. The question is whether that scan is a meaningful cost. Quantified at 60 FPS with peak input rates:

- ~35–40 events per frame in the ring
- Scanning a slot: load discriminator, branch (predictable), advance — ~5 ns per slot (L2 hit)
- Full scan: 40 slots × 5 ns = **200 ns per consumer per frame**
- Frame budget at 60 FPS: 16,667,000 ns
- Scan as percentage of frame: **0.0012%**
- 5 consumers scanning simultaneously: **0.006%**

To reach 1% of frame budget would require ~33,000 events per frame. No real input device generates that volume.

The scan cost is provably negligible. Separate rings add complexity (N rings, up to N read heads per consumer, N independent capacity decisions) with no measurable benefit.

### 2. Grouped Rings (Discrete + Continuous)

**Alternative:** Two rings split by event character — discrete (key press/release, button click) and continuous (cursor position, raw motion, scroll). Coalescing policy maps cleanly onto the continuous ring.

**Rejected because:**
- Same scan-cost argument as above — volume difference does not justify the split at current event rates
- Adds a second ring and a second read head per consumer with no measurable performance benefit
- Coalescing belongs at the adapter or consumer level, not in the ring (see Future Considerations)

### 3. Secondary Index Within the Ring

**Alternative:** Store per-type skip metadata alongside events. Options explored:
- Per-type tail pointer: tracks the most recent write position for each event type
- Linked chain: each slot stores a back-pointer to the previous slot of the same type

**Rejected because:**
- Adds write-path complexity to every `push` call to maintain the index
- The cost being optimized (scan) is ~200 ns/frame — not worth the added write-path work
- Noted as a viable future extension: the linked-chain pattern could be added without changing the ring's external interface, if profiling ever shows scan cost is measurable

### 4. Ring Owns Read Heads

**Alternative:** Ring maintains an internal array of read heads. Consumers register and receive an ID; the ring tracks all positions.

**Rejected because:**
- Under our overflow policy ("overwrite oldest, let slow readers self-detect"), the ring has no use for read head knowledge
- Adds a `MaxSubscribers` compile-time limit or requires dynamic allocation
- Couples the ring to consumer lifecycle management
- Consumer-owned heads are sufficient: each consumer self-detects lapping via `write_head - read_head >= Capacity`

### 5. Seqlock for Torn-Read Protection

**Alternative:** Per-slot sequence lock. Writer increments sequence before and after writing. Reader copies slot data, checks sequence before and after copying, retries if sequence changed.

**Rejected because:**
- Retry loops add unpredictable latency on the read path
- `double_buffer` is already implemented, tested, and battle-proven in `snapshot_cache`
- `double_buffer` provides guaranteed consistent reads with zero retry — correctness is simpler to reason about

### 6. `shared_ptr` for Consumer Access

**Alternative:** Game systems hold `std::shared_ptr<ring_buffer>` instead of a borrowed reference.

**Rejected because:**
- The ring has a single owner (`input_event_bus`). No shared ownership exists.
- `shared_ptr` adds an atomic reference-count operation on every copy and destroy — unnecessary overhead when ownership is not shared
- `std::reference_wrapper` is a single pointer, has zero copy/destroy overhead, and matches the existing JAGE pattern (`jage::game` uses `reference_wrapper` for window and input controller)

---

## Consequences

### Positive

1. **No back-pressure on writer:** `push` is a single `double_buffer` write plus one atomic store. Never blocks regardless of consumer state.
2. **Isolated overflow:** A slow consumer loses only its own events. Other consumers are completely unaffected.
3. **Lock-free:** Publish path: one `double_buffer` write, one atomic store (release). Consumer path: one atomic load (acquire), N `double_buffer` reads. No mutexes anywhere.
4. **Reuses tested primitives:** `double_buffer` per slot and cacheline alignment patterns already proven by `snapshot_cache` and `spsc::queue`.
5. **Negligible memory footprint:** ~48 KB at 256 capacity. Not a meaningful memory concern on any gaming hardware.
6. **Minimal ring interface:** Ring exposes only `push`, `write_head`, and `read`. All drain logic, lap detection, and missed-event handling is consumer-side — each system decides its own policy.

### Negative

1. **Slow consumers silently lose events:** No built-in notification mechanism. Consumers must self-detect lapping by comparing their head to `write_head`.
2. **`double_buffer` memory overhead:** 2× payload per slot. Same tradeoff as `snapshot_cache` — justified by correctness guarantees.
3. **Variant size in single ring:** All slots pay for the size of the largest event type in the variant. Quantified as negligible: `double_buffer` cacheline alignment absorbs most of the size difference between small and large event types.

### Neutral

1. **Consumer drain is boilerplate:** Every consumer writes the same drain loop (load `write_head`, check lap, iterate, update head). Could be extracted as a free-function utility later if the pattern repeats across 2–3 consumers (rule of three).
2. **Capacity is a compile-time decision:** Tuned once per use case. 256 is the recommended starting point for the input event bus.

---

## Future Considerations

### Diagnostics: Observing Consumer Lag

If a diagnostics system needs to observe how far behind each consumer is, read heads migrate from plain `size_t` (consumer-owned, single-thread) to `atomic<size_t>` in a shared, cacheline-padded array. Consumers hold a handle (index) into that array instead of owning the head directly. The ring's interface does not change — only where read heads live.

### Secondary Index for High-Volume Filtering

If profiling ever shows scan cost is measurable (current analysis says it won't be), a per-type linked chain could be added inside slots:
- Each slot stores a back-pointer to the previous slot of the same event type
- Consumers follow the chain for O(k) access where k = events of the target type
- Additive change — does not alter `push`, `write_head`, or `read`

### Drain Utility

If multiple consumers write the same drain loop, extract as a free function:

```cpp
template <typename TEvent, std::size_t Capacity, typename THandler>
auto drain(const ring_buffer<TEvent, Capacity>& ring,
           std::size_t& read_head,
           THandler&& handler) -> void;
```

Add when the pattern repeats across 2–3 consumers.

### Event Coalescing

Coalescing continuous events (summing cursor deltas, keeping latest position) does **not** belong in the ring. The ring is append-only shared storage — coalescing here would destroy data for consumers that need full fidelity (e.g., Replay, Recording).

Coalescing belongs at:
- **Adapter-internal level:** Protects adapter buffers from platform event floods before events enter the bus
- **Consumer level:** Each system decides its own coalescing policy when draining

---

## References

- **Double Buffer:** `include/jage/concurrency/double_buffer.hpp`
- **Cacheline Utilities:** `include/jage/memory/`
- **SPSC Queue (sibling container):** `include/jage/containers/spsc/queue.hpp`
- **Snapshot Cache (similar SPMC ring pattern):** `include/jage/time/snapshot_cache.hpp`
- **Input Adapter Design:** ADR-0003

---

## Revision History

| Date       | Author       | Changes                                      |
|------------|--------------|----------------------------------------------|
| 2026-02-04 | Justin Lye   | Initial ADR from design discussion           |
| 2026-02-05 | Justin Lye   | Updated to match implementation: `push()` method name, `capacity()` function, clarified caller responsibility for index modulo |
