#pragma once

#include <jage/concurrency/double_buffer.hpp>
#include <jage/memory/cacheline_size.hpp>
#include <jage/memory/cacheline_slot.hpp>
#include <jage/time/cache_match_status.hpp>

#include <jage/time/internal/concepts/cache_snapshot.hpp>

#include <array>
#include <atomic>
#include <cstdint>
#include <utility>

namespace jage::time::internal {
template <std::uint64_t Capacity, internal::concepts::cache_snapshot TSnapshot,
          template <class, template <class> class> class TBuffer,
          template <class> class TAtomic>
class snapshot_cache {
  alignas(memory::cacheline_size) std::array<
      memory::cacheline_slot<TBuffer<TSnapshot, TAtomic>>, Capacity> buffer_{};
  alignas(memory::cacheline_size) TAtomic<std::uint64_t> write_index_{0UZ};

public:
  [[nodiscard]] constexpr auto capacity() const noexcept -> std::uint64_t {
    return Capacity;
  }

  auto
  push(const internal::concepts::cache_snapshot auto &input_snapshot) -> void {
    const auto write_index = write_index_.load(std::memory_order::acquire);
    buffer_[write_index % Capacity].write(input_snapshot);
    write_index_.store(write_index + 1, std::memory_order::release);
  }

  [[nodiscard]] auto find(const typename TSnapshot::duration &event_real_time)
      -> std::pair<TSnapshot, cache_match_status> {
    const auto write_index = write_index_.load(std::memory_order::acquire);
    const auto newest_index = (write_index + Capacity - 1UZ) % Capacity;

    for (auto offset = 0UZ; offset < Capacity; ++offset) {
      const auto index = (newest_index + Capacity - offset) % Capacity;
      const auto snap = buffer_[index].read();
      if (event_real_time >= snap.real_time) {
        return {
            snap,
            cache_match_status::matched,
        };
      }
    }

    return {
        buffer_[write_index % Capacity].read(),
        cache_match_status::evicted,
    };
  }

  [[nodiscard]] auto find(const std::uint64_t frame_index)
      -> std::pair<TSnapshot, cache_match_status> {
    const auto write_index = write_index_.load(std::memory_order::acquire);
    const auto newest_index = (write_index + Capacity - 1UZ) % Capacity;
    const auto oldest_index = write_index % Capacity;

    if (const auto newest_snap = buffer_[newest_index].read();
        newest_snap.frame < frame_index) [[unlikely]] {
      return {newest_snap, cache_match_status::ahead};
    } else if (const auto oldest_snap = buffer_[oldest_index].read();
               oldest_snap.frame > frame_index) [[unlikely]] {
      return {oldest_snap, cache_match_status::evicted};
    }
    return {
        buffer_[frame_index % Capacity].read(),
        cache_match_status::matched,
    };
  }
};
} // namespace jage::time::internal