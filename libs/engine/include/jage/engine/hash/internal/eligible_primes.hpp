#pragma once

#include <jage/mp/concepts/unique_values.hpp>

#include <array>
#include <tuple>

namespace jage::engine::hash::internal {

template <auto Prime, auto... Values>
constexpr bool is_eligible = mp::concepts::unique_values<(Values % Prime)...>;

template <auto Primes, auto... Values>
  requires(sizeof...(Values) > 0)
constexpr auto eligible_primes = [] {
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    constexpr auto eligible_prime_count =
        ((is_eligible<Primes[Is], Values...> ? 1UZ : 0UZ) + ...);
    auto result = std::array<typename decltype(Primes)::value_type,
                             eligible_prime_count>{};
    auto index = 0UZ;
    std::ignore =
        ((is_eligible<Primes[Is], Values...> ? (result[index++] = Primes[Is], 0)
                                             : 0),
         ...);
    return result;
  }(std::make_index_sequence<std::size(Primes)>());
};

} // namespace jage::engine::hash::internal