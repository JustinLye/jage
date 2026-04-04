#pragma once

#include <jage/mp/list.hpp>

#include <concepts>
#include <cstdint>
#include <limits>

namespace jage::mp::internal {

template <class...> struct first_index_of {
  static constexpr std::uint16_t value = 0;
};

template <class TTarget, class TCurrent, class... TRest>
struct first_index_of<TTarget, TCurrent, TRest...> {
  static constexpr std::uint16_t value =
      std::same_as<TTarget, TCurrent>
          ? 0
          : static_cast<std::uint16_t>(
                1U + first_index_of<TTarget, TRest...>::value);
};

template <class TTarget, class... Ts>
  requires(sizeof...(Ts) <= std::numeric_limits<std::uint16_t>::max())
struct first_index_of<list<Ts...>, TTarget> : first_index_of<TTarget, Ts...> {};

} // namespace jage::mp::internal
