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