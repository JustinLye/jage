#pragma once

#include <jage/mp/list.hpp>
#include <jage/mp/unique.hpp>

#include <concepts>

namespace jage::mp::internal {

namespace impl_detail {
template <auto V>
  requires std::integral<decltype(V)>
struct integral_type {};

template <auto... Vs> using as_list = list<integral_type<Vs>...>;

} // namespace impl_detail

template <auto... Vs>
constexpr bool unique_values =
    std::same_as<impl_detail::as_list<Vs...>,
                 typename unique<impl_detail::as_list<Vs...>>::type>;

} // namespace jage::mp::internal