#pragma once

#include <jage/mp/concepts/unique_values.hpp>

#include <array>
#include <concepts>
#include <cstdint>
#include <tuple>

namespace jage::engine::hash::internal {

namespace impl_detail {
template <auto Prime, auto... Values>
constexpr bool is_eligible =
    sizeof...(Values) > 0 and mp::concepts::unique_values<(Values % Prime)...>;

template <auto... Values>
constexpr bool all_unsigned_values =
    (std::unsigned_integral<decltype(Values)> and ...);

template <auto... Values>
using value_type = std::conditional_t<all_unsigned_values<Values...>,
                                      std::uint64_t, std::int64_t>;
} // namespace impl_detail

template <auto Primes, auto... Values>
constexpr auto eligible_primes = [] {
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    using namespace impl_detail;
    constexpr auto eligible_prime_count =
        ((is_eligible<Primes[Is], Values...> ? 1UZ : 0UZ) + ...);
    auto result = std::array<value_type<Values...>, eligible_prime_count>{};
    auto index = 0UZ;
    std::ignore =
        ((is_eligible<Primes[Is], Values...> ? (result[index++] = Primes[Is], 0)
                                             : 0),
         ...);
    return result;
  }(std::make_index_sequence<std::size(Primes)>());
};

} // namespace jage::engine::hash::internal