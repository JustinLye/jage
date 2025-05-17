#pragma once

#include <jage/mp/info.hpp>

namespace jage::mp {
template <class TValueType> struct type {
  using value_type = TValueType;
  static void id();

  constexpr auto friend get(info<id>) { return type{}; }
};
} // namespace jage::mp