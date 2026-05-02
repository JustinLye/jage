#pragma once

#include <algorithm>
#include <concepts>
#include <cstdint>
#include <type_traits>

namespace jage::mp {

namespace internal {
template <auto... Vs> struct max_value;
template <> struct max_value<> {
  static constexpr auto value = 0UZ;
};

template <auto... Vs>
  requires(sizeof...(Vs) > 0 and (std::integral<decltype(Vs)> and ...))
struct max_value<Vs...> {

  using common_type =
      std::conditional_t<(std::unsigned_integral<decltype(Vs)> and ...),
                         std::uint64_t, std::int64_t>;
  static constexpr auto value = std::max({static_cast<common_type>(Vs)...});
};

} // namespace internal

template <auto... Vs>
constexpr auto max_value = internal::max_value<Vs...>::value;

} // namespace jage::mp