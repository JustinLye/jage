#pragma once

#include <jage/memory/cacheline_size.hpp>
#include <jage/memory/cacheline_slot.hpp>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>

namespace jage::containers::spsc {
template <class TEvent, std::size_t Capacity,
          template <class> class TAtomic = std::atomic>
class alignas(memory::cacheline_size) queue {
  alignas(memory::cacheline_size) TAtomic<std::uint64_t> head_{0UZ};
  alignas(memory::cacheline_size) TAtomic<std::uint64_t> tail_{0UZ};
  alignas(memory::cacheline_size)
      std::array<memory::cacheline_slot<TEvent>, Capacity> buffer_{};

public:
  using value_type = TEvent;
  [[nodiscard]] auto empty() const -> bool {
    return head_.load(std::memory_order::acquire) ==
           tail_.load(std::memory_order::acquire);
  }

  [[nodiscard]] auto size() const -> std::size_t {
    return std::min(tail_.load(std::memory_order::acquire) -
                        head_.load(std::memory_order::acquire),
                    Capacity);
  }
  [[nodiscard]] static constexpr auto capacity() -> std::size_t {
    return Capacity;
  }

  auto push(TEvent &&event) -> void {
    const auto tail_index = tail_.load(std::memory_order::acquire);
    if (auto head_index = head_.load(std::memory_order::acquire);
        tail_index - head_index >= Capacity) {
      const auto desired_head = head_index + 1UZ;
      while (not head_.compare_exchange_weak(head_index, desired_head,
                                             std::memory_order::release,
                                             std::memory_order::acquire) and
             head_index < desired_head) {
      }
    }
    buffer_[tail_index % Capacity] = std::forward<decltype(event)>(event);
    tail_.store(tail_index + 1, std::memory_order::release);
  }

  [[nodiscard]] auto front() const -> TEvent {
    return buffer_[head_.load(std::memory_order::acquire) % Capacity];
  }

  auto pop() -> void {
    if (const auto current_head = head_.load(std::memory_order::acquire);
        current_head == tail_.load(std::memory_order::acquire)) [[unlikely]] {
      return;
    } else {
      head_.store(current_head + 1, std::memory_order::release);
    }
  }
};

} // namespace jage::containers::spsc