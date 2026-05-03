#pragma once

#include <jage/mp/concepts/unique_values.hpp>

#include <array>
#include <cstddef>
#include <tuple>
#include <utility>

namespace jage::engine::hash::internal {

template <auto Primes>
constexpr auto indices = std::make_index_sequence<std::size(Primes)>();

template <auto Prime, auto... Values>
constexpr bool is_eligible = mp::concepts::unique_values<(Values % Prime)...>;

template <auto Primes, auto... Values>
constexpr auto count_eligible_primes =
    []<std::size_t... Is>(std::index_sequence<Is...>) -> std::size_t {
  return ((is_eligible<Primes[Is], Values...> ? 1UZ : 0UZ) + ...);
}(indices<Primes>);

template <auto Primes, auto... Values>
using result_type = std::array<typename decltype(Primes)::value_type,
                               count_eligible_primes<Primes, Values...>>;

template <auto Primes, auto... Values, std::size_t... Is>
constexpr auto make_eligible_primes(std::index_sequence<Is...>)
    -> result_type<Primes, Values...> {
  auto result = result_type<Primes, Values...>{};
  auto index = 0UZ;
  std::ignore =
      ((is_eligible<Primes[Is], Values...> ? (result[index++] = Primes[Is], 0)
                                           : 0),
       ...);
  return result;
};

template <auto Primes, auto... Values>
  requires(sizeof...(Values) > 0)
constexpr auto eligible_primes = [] -> result_type<Primes, Values...> {
  return make_eligible_primes<Primes, Values...>(indices<Primes>);
}();

} // namespace jage::engine::hash::internal