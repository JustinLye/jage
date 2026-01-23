#pragma once
#include <gmock/gmock.h>

#include <atomic>
#include <cstdint>
#include <memory>

namespace jage::test::mocks::concurrency {

template <class T> struct atomic {
  static std::shared_ptr<atomic> instance;
  [[nodiscard]] static auto get_instance() -> std::shared_ptr<atomic> {
    if (nullptr == instance) {
      instance = std::make_shared<atomic>();
    }
    return instance;
  }

  MOCK_METHOD(std::uint8_t, mock_load, (std::memory_order), (const noexcept));
  MOCK_METHOD(void, mock_store, (std::uint8_t, std::memory_order), (noexcept));

  atomic() = default;
  atomic(std::uint8_t) {}
  [[nodiscard]] static auto
  load(std::memory_order order) noexcept -> std::uint8_t {
    return get_instance()->mock_load(order);
  }

  static auto store(std::uint8_t desired,
                    std::memory_order order) noexcept -> void {
    get_instance()->mock_store(desired, order);
  }
};

template <class T> std::shared_ptr<atomic<T>> atomic<T>::instance = nullptr;

} // namespace jage::test::mocks::concurrency