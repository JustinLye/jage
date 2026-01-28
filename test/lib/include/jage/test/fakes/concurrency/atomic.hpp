#pragma once

#include <atomic>
#include <stdexcept>

namespace jage::test::fakes::concurrency {
template <class TValue,
          std::memory_order LoadOrder = std::memory_order::acquire,
          std::memory_order StoreOrder = std::memory_order::release>
struct atomic {
  TValue value{};
  [[nodiscard]] auto load(const std::memory_order order) const -> TValue {
    if (order != LoadOrder) {
      throw std::invalid_argument("Invalid load order");
    }
    return value;
  }

  auto store(TValue desired, std::memory_order order) -> void {
    if (order != StoreOrder) {
      throw std::invalid_argument("Invalid store order");
    }
    value = desired;
  }

  [[nodiscard]] auto compare_exchange_weak(TValue &expected, TValue desired,
                                           std::memory_order success,
                                           std::memory_order failure) -> bool {
    if (success != StoreOrder or failure != LoadOrder) {
      throw std::invalid_argument("Invalid store and/or load order");
    }
    if (value == expected) {
      value = desired;
      return true;
    }
    expected = value;
    return false;
  }
};
} // namespace jage::test::fakes::concurrency