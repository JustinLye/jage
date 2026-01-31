# What do I need out of the underlying container used by snapshot cache.

_Below is a prompt I prepared for Calude. Wanted to hang on to this in-case I need to provided it an AI agent again (and I guess I'm a bit of a horder)_

Cool, I'm thinking about the design of the snapshot cache. I describe the snapshot cache in a github milestone: https://github.com/JustinLye/jage/milestone/2. Have we discussed the Snapshot Cache yet? In not review the milestone. 

So one topic I have been mulling around is how will I store snapshots. Specifically, what kind of data structure should I use? I want the structure to be a fixed size ring. I think size to use can be determined overtime. We can increase it and lower it. We can put diagnostics around it to measure the performance of various sizes, but right now that doesn't matter. 

I will need to peform look ups, specifically I will need to find a snapshot by real-timestamp and I will need to find a snapshot by frame index. I can use an offset when looking up by frame index, so complexity is O(1). To find a snapshot by real-timestamp, I think I will just do a linear search for name O(N). Perhaps later, with some diagnostic data in-hand, I can optimize the search; however, I want to focus on cache usage efficiency, so the underlying container will be a std::array. Speaking of underlying containers, I was thinking about the jage::containers::queue. I like that is is cache aligned and I like that it prevents false sharing. I'm thinking I might be able to either create something similar, like a jage::containers::array. The jage::containers::queue is really close to what I want, but it doesn't have an `operator[]` (or `at()`). Maybe, I could template the jage::containers::queue on a TContainer type, where jage::containers::array is the default. This way I'm reusing some code.

Anyway, back to the task at hand. I think the snapshot cache will consume snapshot events, which it will write to a private buffer data member. So the snapshot cache will need

```cpp
namespace jage::time {
class snapshot_cache {
    type_tbd buffer;
    public:
        auto push(internal::events::snapshot) -> void;

};
}
```



I'm using `type_tbd` because the type of the buffer is what we are discussing. Also, as a side note I should probably move `jage::time::internal::events::snapshot` to `jage::time::events::snapshot`.

The snaphot cache will need to provide a query interface to allow lookup by frame index and lookup by real-timestamp. I don't have one yet, but assume `jage::time::concepts::duration` exists and cannot be confused with std::uint64_t, which is the type of the frame index (a.k.a. `snapshot::ticks`)

Adding the query interface, the `snapshot_cache` might look something like this:

```cpp
namespace jage::time {
class snapshot_cache {
        type_tbd buffer;
    public:
        auto push(internal::events::snapshot) -> void;
        auto find(const concepts::duration auto&) -> internal::events::snapshot;
        auto find(const std::uint64_t) -> internal::events::snapshot;
};
}
```

As discussed in the milestone, we might return additional information when performing a cache query. So, perhaps we have something like:

```cpp
namespace jage {
    enum class accuracy : std::uint8_t {
        exact,
        estimate
    };
}
```

We would now return some pair or tuple from find:

```cpp
namespace jage::time {
class snapshot_cache {
        type_tbd buffer;
    public:
        auto push(internal::events::snapshot) -> void;
        auto find(const concepts::duration auto&) -> std::pair<internal::events::snapshot, accuracy>;
        auto find(const std::uint64_t) -> std::pair<internal::events::snapshot, accuracy>;
};
}
```

As of right now, I think that is the minimum interface we will need.

So, a single producer will be calling `push`, but many other components on other threads will be calling `find`. `type_tbd` will need to be single producer-multiple consumer. I also want it to be lock-free (if possible), cache alignment aware, and prevent false sharing. 

Now that I think about it, `jage::containers::queue`. If I use the queue, snapshot_cache would look like:

```cpp
namespace jage::time {
    template<auto Size>
    class snapshot_cache {
        containers::queue<internal::events::snapshot, Size, std::atomic> buffer;
    public:
        auto push(internal::events::snapshot) -> void;
        auto find(const concepts::duration auto&) -> std::pair<internal::events::snapshot, accuracy>;
        auto find(const std::uint64_t) -> std::pair<internal::events::snapshot, accuracy>;
    };
}
```

The snapshot_cache will be the only one writing to the buffer, but it is also the only one consuming it. A really basic (semi-pseudo) implementation of `push` could be something like:


```cpp
namespace jage::time {
    template<auto Size>
    class snapshot_cache {
        containers::queue<internal::events::snapshot, Size, std::atomic> buffer;
    public:
        auto push(internal::events::snapshot snapshot) -> void {
            buffer.push(snapshot);
        }
        auto find(const concepts::duration auto&) -> std::pair<internal::events::snapshot, accuracy>;
        auto find(const std::uint64_t) -> std::pair<internal::events::snapshot, accuracy>;
    };
}
```

Just forwarding the push allows the head to be overwritten; however, we will cause the `buffer` to repeatedly perform its' CAS loop. We really need a structure that just has a next write position, but it would support an interface like.

```cpp
namespace jage::containers {
    template<class T, std::size_t Capacity>
    class ring_buffer {
        public:
          auto write(T) -> void;
          static constexpr auto capacity() const -> std::size_t { return Capacity; }
          auto operator[](const std::size_t index) const -> T;
          auto at(const std::size_t index) const -> T;
    }
}
```

Perhaps the `operator[]` could return an `optional`, but I'm concerned about the performance implications for having to check the optional value here. I think it may be best just to return a default value of T.

A type like `ring_buffer` isn't really consumed; however, its' not well organaized. When it comes to storing snapshots, we would probably want to start a search with the oldest element in the container. Just looping through and calling `operator[]` or `at` from 0 to `capacity()` could lead to missing snapshots that may have been found if we started with the oldest.

The `snapshot_cache` could use `jage::containers::queue`, but on construction, `snapshot_cache` would fill the queue with default `snapshots`, The it can `pop()` (a.k.a. evict) the oldest item, before pushing a new one on. This shoud avoid any contention because the queue will never be at capacity when a new snapshot is pushed.

The implementation might look something like this:

```cpp
namespace jage::time {
    template<auto Size>
    class snapshot_cache {
        containers::queue<internal::events::snapshot, Size, std::atomic> buffer;
    public:
        snapshot_cache() {
            auto rg = std::array<internal::events::snapshot, Size>{};
        #ifdef __cpp_lib_containers_ranges
            buffer.push_range(std::move(rg))
        #else
            for (auto i : rg) {
                buffer.push(i);
            }
        #endif
        }
        auto push(internal::events::snapshot snapshot) -> void {
            buffer.pop();
            buffer.push(snapshot);
        }
        auto find(const concepts::duration auto&) -> std::pair<internal::events::snapshot, accuracy>;
        auto find(const std::uint64_t) -> std::pair<internal::events::snapshot, accuracy>;
    };
}
```

I know `queue` does not `push_range`, but I can add that.

I'm also wondering if we need to atomically read from the buffer when we are doing `find`. If so, I think wrapping the snapshot in a double buffer would help; although, it would double the size of the snapshot cache. Anyway, `snapshot_cache` would look something like:

```cpp
namespace jage::time {
    template<auto Size>
    class snapshot_cache {
        containers::queue<concurrency::double_buffer<internal::events::snapshot>, Size, std::atomic> buffer;
    public:
        snapshot_cache() {
            auto rg = std::array<internal::events::snapshot, Size>{};
        #ifdef __cpp_lib_containers_ranges
            buffer.push_range(std::move(rg))
        #else
            for (auto i : rg) {
                buffer.push(i);
            }
        #endif
        }
        auto push(internal::events::snapshot snapshot) -> void {
            buffer.pop();
            buffer.push(snapshot);
        }
        auto find(const concepts::duration auto&) -> std::pair<internal::events::snapshot, accuracy>;
        auto find(const std::uint64_t) -> std::pair<internal::events::snapshot, accuracy>;
    };
}
```

If I did you `double_buffer` I would need to add an assignment operator so that `queue::push` will compile.

One problem I see with using `queue` is there's no way of querying past the first element in the queue. I could add `queue::operator[]` and `queue::at` methods, but then its' not really a queue anymore. Perhaps I can create a new data structure like 

```cpp
namespace jage::containers {

    template<class T, std::size_t Capacity, template<class> class TAtomic = std::atomic>
    class ring_buffer : public queue<T, Capacity, TAtomic> {

    public:
        auto operator[](std::size_t) const -> T;
        auto at(std::size_t) const -> T;
    };
}
```

Of course, in order for that to work I would have to expose `head_`, in `queue`, by moving it from private to protected. Maybe I would just add a protected method to queue

```cpp
auto head() -> std::uint64_t { return head_.load(std::memory_order::acquire); }
```

Actually, that doesn't work, because `ring_buffer` would still need access to `queue::buffer_`.

Hmmm. I don't really want to add `operator[]` and `at` to `queue`. Maybe, `ring_buffer` will need its' own implementation, or perhaps a base class with common functionality can be created and `queue` and `ring_buffer` are derived from that and expose whatever interface makes since for a `queue` or `ring_buffer`.

Okay, this should provide us some momentum. Please share your thoughts.

---

# Claude's Architectural Response (2026-01-30)

## The Core Problem: Queue is the Wrong Abstraction

`★ Insight ─────────────────────────────────────`
**A queue is FIFO—you consume from the head and discard.** But your snapshot cache needs:
1. Write to tail (producer pushes new snapshots)
2. **Read from anywhere** (find by timestamp/index)
3. **Never consume** (queries don't remove snapshots)

This is a **circular buffer with indexed reads**, not a queue.
`─────────────────────────────────────────────────`

**Why the queue approach feels awkward:**
- You're pre-filling to avoid contention → fighting the abstraction
- You want `operator[]` → queues don't expose indexed access
- You're doing `pop()` then `push()` → simulating circular overwrite, not queue semantics

**My Challenge:** Don't extend `queue`. Create a dedicated `ring_buffer` designed for SPMC with random read access.

---

## SPMC Lock-Free: Harder Than It Seems

Your current `jage::containers::queue` is **SPSC** (single producer, single consumer). SPMC requires different synchronization guarantees.

**The Key Questions:**
1. **Do readers need strongly consistent snapshots?**
   - If a reader is searching the buffer and the writer overwrites an entry mid-search, what happens?
   - For time snapshots, I suspect **eventual consistency is acceptable**—getting a snapshot that's a few microseconds stale is fine.

2. **Do you need read-your-writes guarantees?**
   - After the producer writes a snapshot, when must readers see it?
   - For diagnostics/replay, you probably need "reasonably soon" (within microseconds), not "immediately."

**For SPMC, you have options:**

### **Option A: Atomic Index + Memory Ordering (Recommended)**
- Producer updates an atomic `write_index` after writing
- Readers read `write_index` to determine valid range
- Use `std::memory_order::release` on write, `std::memory_order::acquire` on read
- **Trade-off:** Readers may see slightly stale data during buffer updates

### **Option B: Sequence Lock (Seqlock) Pattern**
- Producer increments a sequence number before and after writing
- Readers check sequence: if odd (write in progress) or changed during read, retry
- **Trade-off:** More complex, but gives stronger consistency guarantees

### **Option C: Double Buffer Per Slot (Your Idea)**
- Wrap each snapshot in `double_buffer<snapshot>`
- **Trade-off:** Doubles memory usage (~60-70 bytes × 2 × capacity). For 120 snapshots, that's ~8-16KB extra—probably acceptable, but feels wasteful.

**My Recommendation:** **Option A** for simplicity. Eventual consistency is fine for time snapshots.

---

## Proposed Design: Dedicated Ring Buffer

Instead of extending `queue`, let's design a purpose-built `ring_buffer` for SPMC:

```cpp
namespace jage::containers {

template<class T, std::size_t Capacity>
class ring_buffer {
    static_assert(Capacity > 0, "Capacity must be positive");
    static_assert(std::is_trivially_copyable_v<T>,
                  "T must be trivially copyable for lock-free SPMC");

    // Cacheline-aligned storage
    alignas(memory::cacheline_size) std::array<T, Capacity> buffer_{};

    // Atomic write index (only producer updates this)
    alignas(memory::cacheline_size) std::atomic<std::size_t> write_index_{0};

public:
    // Producer API (single writer)
    auto push(T value) -> void {
        auto index = write_index_.load(std::memory_order::relaxed);
        buffer_[index % Capacity] = value;

        // Release semantics: ensure write to buffer_ is visible before index update
        write_index_.store(index + 1, std::memory_order::release);
    }

    // Consumer API (multiple readers)
    [[nodiscard]] auto operator[](std::size_t index) const -> T {
        // Acquire semantics: ensure we see the latest buffer_ writes
        auto current_write = write_index_.load(std::memory_order::acquire);

        // Calculate actual buffer position
        return buffer_[index % Capacity];
    }

    [[nodiscard]] auto at(std::size_t index) const -> std::optional<T> {
        auto current_write = write_index_.load(std::memory_order::acquire);
        auto oldest_valid = (current_write >= Capacity) ? (current_write - Capacity) : 0;

        // Check if index is in valid range
        if (index < oldest_valid || index >= current_write) {
            return std::nullopt;
        }

        return buffer_[index % Capacity];
    }

    // Accessors
    [[nodiscard]] static constexpr auto capacity() -> std::size_t {
        return Capacity;
    }

    [[nodiscard]] auto size() const -> std::size_t {
        auto current_write = write_index_.load(std::memory_order::acquire);
        return std::min(current_write, Capacity);
    }

    // Get valid index range for iteration
    [[nodiscard]] auto valid_range() const -> std::pair<std::size_t, std::size_t> {
        auto current_write = write_index_.load(std::memory_order::acquire);
        auto oldest = (current_write >= Capacity) ? (current_write - Capacity) : 0;
        return {oldest, current_write};
    }
};

} // namespace jage::containers
```

---

## Why This Design Works for SPMC

`★ Insight ─────────────────────────────────────`
**Lock-free SPMC relies on atomic ordering, not locks:**
1. **Producer** uses `release` semantics—guarantees buffer writes complete before index update
2. **Consumers** use `acquire` semantics—guarantees they see buffer writes after reading index
3. **Cacheline alignment** prevents false sharing between write_index and buffer
`─────────────────────────────────────────────────`

**Trade-offs:**
- ✅ No locks, no contention
- ✅ Readers never block the writer
- ✅ Simple, cache-efficient (contiguous storage)
- ⚠️ Readers may see stale snapshots during writes (acceptable for time data)
- ⚠️ Requires `T` to be trivially copyable (fine for `snapshot`)

---

## Snapshot Cache Implementation

With this `ring_buffer`, your `snapshot_cache` becomes clean:

```cpp
namespace jage::time {

template<std::size_t Size>
class snapshot_cache {
    containers::ring_buffer<events::snapshot, Size> buffer_;

public:
    // Producer API (called by clock on main thread)
    auto push(events::snapshot snapshot) -> void {
        buffer_.push(snapshot);
    }

    // Consumer API: Find by frame index
    [[nodiscard]] auto find(std::uint64_t frame_index) const
        -> std::pair<events::snapshot, accuracy>
    {
        auto [oldest, newest] = buffer_.valid_range();

        // Out of range: return boundary
        if (frame_index < oldest) {
            return {buffer_[oldest], accuracy::estimate};
        }
        if (frame_index >= newest) {
            return {buffer_[newest - 1], accuracy::estimate};
        }

        // Exact match
        return {buffer_[frame_index], accuracy::exact};
    }

    // Consumer API: Find by real-timestamp
    template<concepts::duration Duration>
    [[nodiscard]] auto find(const Duration& timestamp) const
        -> std::pair<events::snapshot, accuracy>
    {
        auto [oldest, newest] = buffer_.valid_range();

        // Linear search (backward from newest for recency)
        for (auto i = newest; i > oldest; --i) {
            auto snapshot = buffer_[i - 1];
            if (snapshot.real_time() <= timestamp) {
                // Found: latest snapshot not exceeding timestamp
                auto is_exact = (snapshot.real_time() == timestamp);
                return {snapshot, is_exact ? accuracy::exact : accuracy::estimate};
            }
        }

        // Timestamp predates all snapshots
        return {buffer_[oldest], accuracy::estimate};
    }
};

} // namespace jage::time
```

---

## Addressing Specific Questions

### **1. Do we need atomic reads for `find()`?**

**Answer:** With the `memory_order::acquire` on `write_index_`, you get sufficient guarantees:
- Readers see a consistent snapshot (no torn reads)
- Readers may see slightly stale data (writer overwrites during search)

For time snapshots, this is acceptable. If you need stronger guarantees, use **Option C** (double buffer per slot), but I don't think it's necessary.

---

### **2. Should we extend `queue` or create `ring_buffer`?**

**Answer:** **Create a dedicated `ring_buffer`.**

**Why not extend queue:**
- Different access patterns (FIFO vs random read)
- Different synchronization (SPSC vs SPMC)
- Extending queue pollutes its interface (`operator[]` on a queue is confusing)
- Inheritance for code reuse is often a smell—prefer composition or separate implementations

**Why not a base class:**
- `queue` and `ring_buffer` have little shared code beyond "array + atomic index"
- Premature abstraction adds complexity without clear benefit
- If you later find duplication, **refactor then**, not now

---

### **3. Linear search performance concerns?**

For **60-120 snapshots** (1-2 seconds at 60 FPS):
- Linear search is ~60-120 comparisons (nanoseconds on modern CPUs)
- Cache-friendly (contiguous array, prefetcher loves this)
- Searching backward from newest improves average case (recent timestamps more likely)

**Optimization options later (with data):**
- Binary search (if timestamps are monotonic—they should be!)
- Bloom filter for "not present" queries
- Small hash table (index % small_prime) for common lookups

**Don't optimize now.** Get diagnostics first.

---

### **4. Moving `internal::events::snapshot` to `events::snapshot`?**

**Yes, do it.** `internal` suggests "don't touch," but if snapshot cache (a public API) returns `snapshot`, it's part of your public interface. Either:
- Move to `jage::time::events::snapshot` (clear namespace)
- Or `jage::time::snapshot` if "event" is implied by context

---

## Challenge: Is `std::array` the Right Size?

You mentioned wanting a fixed-size ring. **How big should it be?**

**Some size guidelines:**
- **60 snapshots** = 1 second at 60 FPS (~4KB if snapshot is 60 bytes)
- **120 snapshots** = 2 seconds (~8KB)
- **600 snapshots** = 10 seconds (~40KB)

**Question for you:** What's your expected query latency?
- If input events arrive within 1 frame (~16ms at 60 FPS), 60 snapshots is plenty
- If you're replaying recorded input from 10 seconds ago, you need a deeper cache

**My suggestion:** Start with `120` as a template parameter default. Add diagnostics to measure:
- Cache hit rate (exact vs estimate)
- Query timestamp age distribution
- Oldest snapshot ever queried

Then tune based on data.

---

## Future Diagnostic Ideas (Low Priority)

**Metrics to Track (Later):**
1. **Cache hit rate**: Percentage of queries returning `accuracy::exact` vs `accuracy::estimate`
2. **Query age distribution**: Histogram of (current_time - queried_timestamp)
   - Tells you how far back in time queries typically go
   - Helps determine optimal cache size
3. **Oldest snapshot queried**: Track max age of any queried snapshot
   - If you never query beyond 1 second, you're over-provisioned
4. **Overwrite frequency**: How often does the cache wrap around?
   - High frequency + low hit rate = cache too small
5. **Search depth**: For timestamp queries, how many iterations in the linear search?
   - Avg/max/p95 search depth helps decide if binary search is worth it

**Instrumentation Strategy:**
```cpp
template<std::size_t Size>
class snapshot_cache {
    containers::ring_buffer<events::snapshot, Size> buffer_;

    // Optional diagnostics (compile-time enabled)
    #ifdef JAGE_ENABLE_SNAPSHOT_CACHE_DIAGNOSTICS
    mutable std::atomic<std::size_t> exact_hits_{0};
    mutable std::atomic<std::size_t> estimate_hits_{0};
    mutable std::atomic<std::size_t> total_search_iterations_{0};
    #endif

public:
    // ... existing methods ...

    #ifdef JAGE_ENABLE_SNAPSHOT_CACHE_DIAGNOSTICS
    auto diagnostics() const -> snapshot_cache_stats {
        return {
            .exact_hits = exact_hits_.load(),
            .estimate_hits = estimate_hits_.load(),
            .hit_rate = static_cast<double>(exact_hits_) / (exact_hits_ + estimate_hits_),
            .avg_search_depth = static_cast<double>(total_search_iterations_) / (exact_hits_ + estimate_hits_)
        };
    }
    #endif
};
```

**Binary Search Optimization (If Data Shows Need):**
Since snapshots are monotonically increasing by timestamp, you can binary search:
```cpp
template<concepts::duration Duration>
[[nodiscard]] auto find_binary(const Duration& timestamp) const
    -> std::pair<events::snapshot, accuracy>
{
    auto [oldest, newest] = buffer_.valid_range();

    // Binary search for latest snapshot <= timestamp
    auto left = oldest;
    auto right = newest;

    while (left < right) {
        auto mid = left + (right - left) / 2;
        auto snapshot = buffer_[mid];

        if (snapshot.real_time() <= timestamp) {
            left = mid + 1;  // Look in right half
        } else {
            right = mid;     // Look in left half
        }
    }

    // left-1 is the latest snapshot not exceeding timestamp
    if (left == oldest) {
        return {buffer_[oldest], accuracy::estimate};  // Too old
    }

    auto result = buffer_[left - 1];
    auto is_exact = (result.real_time() == timestamp);
    return {result, is_exact ? accuracy::exact : accuracy::estimate};
}
```
O(log N) vs O(N), but only matters if N > ~100 and profiling shows it's a bottleneck.

---

## Next Steps (Recommended Implementation Order)

1. **Create `jage::containers::ring_buffer`** with the SPMC design above
2. **Implement `snapshot_cache`** using the ring buffer
3. **Write tests** for:
   - Single producer pushing snapshots
   - Multiple consumers querying concurrently (spawn threads)
   - Out-of-range queries return boundaries with `accuracy::estimate`
   - Frame index and timestamp lookups both work
4. **Add diagnostics** (later) to measure cache efficiency

**A question for you:**
Does the producer (clock) need to know when the cache is full, or should it always overwrite the oldest? I assume overwrite, but confirming.