#pragma once
#include <jage/memory/cacheline_size.hpp>

#include <jage/concurrency/internal/concepts/buffer.hpp>

#include <array>
#include <atomic>
#include <cstddef>

#if defined(JAGE_ENABLE_SANITY_CHECKS) and JAGE_ENABLE_SANITY_CHECKS == 1
#include <stdexcept>
#endif

namespace jage::containers::spmc::internal {
template <class TEvent, std::size_t Capacity, template <class> class TAtomic,
          template <class, template <class> class> class TBuffer>
  requires(concurrency::internal::concepts::buffer<TBuffer<TEvent, TAtomic>>)
class ring_buffer {
  alignas(memory::cacheline_size)
      std::array<TBuffer<TEvent, TAtomic>, Capacity> buffer_;
  alignas(memory::cacheline_size) TAtomic<std::size_t> write_head_;

public:
  [[nodiscard]] static constexpr auto capacity() -> std::size_t {
    return Capacity;
  }

  [[nodiscard]] constexpr auto write_head() const -> std::size_t {
    return write_head_.load(std::memory_order::acquire);
  }

  constexpr auto push(const TEvent &event) -> void {
    const auto head = write_head_.load(std::memory_order::relaxed);
    buffer_[head % Capacity].write(event);
    write_head_.store(head + 1, std::memory_order::release);
  }

  [[nodiscard]] auto read(const std::size_t index) const -> TEvent {
#if defined(JAGE_ENABLE_SANITY_CHECKS) and JAGE_ENABLE_SANITY_CHECKS == 1
    if (index >= Capacity) {
      throw std::invalid_argument{
          "Index is greater than capacity of ring buffer"};
    }
#endif
    return buffer_[index].read();
  }
};
} // namespace jage::containers::spmc::internal