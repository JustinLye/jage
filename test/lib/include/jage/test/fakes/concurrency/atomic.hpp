#pragma once

#include <atomic>

namespace jage::test::fakes::concurrency {
template <class TValue> struct atomic {
  TValue value{};
  [[nodiscard]] auto load(const std::memory_order) const -> TValue {
    return value;
  }

  auto store(TValue desired, std::memory_order) -> void { value = desired; }

  [[nodiscard]] auto compare_exchange_weak(TValue &expected, TValue desired,
                                           std::memory_order,
                                           std::memory_order) -> bool {
    if (value == expected) {
      value = desired;
      return true;
    }
    expected = value;
    return false;
  }
};
} // namespace jage::test::fakes::concurrency