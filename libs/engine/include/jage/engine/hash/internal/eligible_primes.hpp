#pragma once

#include <jage/mp/concepts/unique_values.hpp>

#include <array>
#include <bitset>
#include <concepts>
#include <cstdint>
#include <optional>

namespace jage::engine::hash::internal {

template <auto... Vs> struct value_pack {};

template <class> constexpr bool is_array = false;

template <class U, auto N> constexpr bool is_array<std::array<U, N>> = true;

template <auto Prime, auto... Vs>
  requires(not is_array<decltype(Prime)> and std::integral<decltype(Prime)>)
constexpr bool is_eligible = mp::concepts::unique_values<(Vs % Prime)...>;

template <auto Prime> constexpr bool is_eligible<Prime> = false;

static_assert(is_eligible<7, 100, 101, 103>);
static_assert(is_eligible<7, 100, 101, 103, 200>);

template <auto, class, auto...> struct set_insert;

template <auto Values, auto... T> struct set_insert<Values, value_pack<T...>> {
  using type = value_pack<T...>;
};

template <auto Values, auto... Unique, auto Candidate,
          auto... RemainingCandidates>
  requires(is_array<decltype(Values)>)
struct set_insert<Values, value_pack<Unique...>, Candidate,
                  RemainingCandidates...> {
  using current_set_type = std::conditional_t<is_eligible<Candidate, Values>,
                                              value_pack<Unique..., Candidate>,
                                              value_pack<Unique...>>;
  using type =
      set_insert<Values, current_set_type, RemainingCandidates...>::type;
};

template <auto Primes, auto... Values>
  requires(is_array<decltype(Primes)>)
static constexpr auto size_of_eligible_primes = [] {
  return []<std::size_t... Is>(std::index_sequence<Is...>) {
    return ((is_eligible<Primes[Is], Values...> ? 1UZ : 0UZ) + ...);
  }(std::make_index_sequence<std::size(Primes)>());
}();

template <auto... Values>
using common_type =
    std::conditional_t<(std::unsigned_integral<decltype(Values)> and ...),
                       std::uint64_t, std::int64_t>;

template <auto Primes, auto... Values>
static constexpr bool has_eligible_primes =
    0UZ < size_of_eligible_primes<Primes, Values...>;

template <auto Primes, auto... Values> struct eligible_primes {
  static constexpr auto value =
      std::optional<std::array<common_type<Values...>, 1>>{};
};

template <auto Primes, auto... Values>
  requires(has_eligible_primes<Primes, Values...>)
struct eligible_primes<Primes, Values...> {
  static constexpr auto primes_size = std::size(Primes);
  static constexpr auto eligible_prime_mask = std::bitset<std::size(Primes)>{
      []<std::size_t... Is>(std::index_sequence<Is...>) {
        return 0UZ |
               ((is_eligible<Primes[Is], Values...> ? (1UZ << Is) : 0UZ) | ...);
      }(std::make_index_sequence<std::size(Primes)>())};

  using array_type = std::array<common_type<Values...>,
                                size_of_eligible_primes<Primes, Values...>>;

  static constexpr auto value = [] -> array_type {
    auto primes = array_type{};
    auto offset = 0UZ;
    for (auto i = 0UZ; i < std::size(Primes); ++i) {
      if (eligible_prime_mask.test(i)) {
        primes[i - offset] = Primes[i];
      } else {
        ++offset;
      }
    }
    return primes;
  }();
};

template <auto, auto...> static constexpr auto try_get_eligible_primes() {
  return std::optional<int>{};
}

template <auto Primes, auto... Values>
  requires(has_eligible_primes<Primes, Values...>)
static constexpr auto try_get_eligible_primes() {
  return std::optional{eligible_primes<Primes, Values...>::value};
}

} // namespace jage::engine::hash::internal