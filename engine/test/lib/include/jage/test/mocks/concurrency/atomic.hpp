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

  MOCK_METHOD(std::uint64_t, mock_load, (std::memory_order), (const noexcept));
  MOCK_METHOD(void, mock_store, (std::uint64_t, std::memory_order), (noexcept));
  MOCK_METHOD(bool, mock_compare_exchange_weak,
              (std::uint64_t &, std::uint64_t, std::memory_order,
               std::memory_order));

  atomic() = default;
  atomic(std::uint64_t) {}
  [[nodiscard]] static auto
  load(std::memory_order order) noexcept -> std::uint64_t {
    return get_instance()->mock_load(order);
  }

  static auto store(std::uint64_t desired,
                    std::memory_order order) noexcept -> void {
    get_instance()->mock_store(desired, order);
  }

  [[nodiscard]] static auto
  compare_exchange_weak(std::uint64_t &expected, std::uint64_t desired,
                        std::memory_order success,
                        std::memory_order failure) -> bool {
    return get_instance()->mock_compare_exchange_weak(expected, desired,
                                                      success, failure);
  }
};

template <class T> std::shared_ptr<atomic<T>> atomic<T>::instance = nullptr;

} // namespace jage::test::mocks::concurrency