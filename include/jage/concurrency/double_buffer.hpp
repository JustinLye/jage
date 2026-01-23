#pragma once

#include <jage/memory/cacheline_size.hpp>
#include <jage/memory/cacheline_slot.hpp>

#include <array>
#include <atomic>
#include <cstdint>

namespace jage::concurrency {
template <class T, template <class> class TAtomic = std::atomic>
class alignas(memory::cacheline_size) double_buffer {
  alignas(
      memory::cacheline_size) std::array<memory::cacheline_slot<T>, 2> buffer_;
  alignas(memory::cacheline_size) TAtomic<std::uint8_t> index_{0U};

  static_assert(alignof(decltype(buffer_)) >= memory::cacheline_size);
  static_assert(sizeof(decltype(buffer_)) % memory::cacheline_size == 0);

public:
  [[nodiscard]] auto read() const -> T {
    const auto active_index = index_.load(std::memory_order::acquire);
    return buffer_[active_index];
  }

  auto write(const T &desired) -> void {
    const auto active_index = index_.load(std::memory_order::acquire);
    const auto inactive_index = static_cast<std::uint8_t>(active_index ^ 1U);
    buffer_[inactive_index] = desired;
    index_.store(inactive_index, std::memory_order::release);
  }
};
} // namespace jage::concurrency