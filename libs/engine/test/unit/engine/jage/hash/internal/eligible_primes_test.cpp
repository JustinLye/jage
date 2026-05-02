#include <jage/engine/hash/internal/eligible_primes.hpp>

#include <gtest/gtest.h>

#include <array>

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_select_eligible_primes_for_hashing_finite_set) {

  using jage::engine::hash::internal::eligible_primes;
  using jage::engine::hash::internal::try_get_eligible_primes;

  static constexpr auto test_primes = std::array{
      7UZ, 11UZ, 13UZ, 17UZ, 19UZ, 23UZ,
  };

  static_assert(test_primes == eligible_primes<test_primes, 1UZ>::value);
  static_assert(std::array{11UZ, 13UZ, 17UZ, 19UZ, 23UZ} ==
                eligible_primes<test_primes, 7UZ, 14UZ>::value);

  static_assert(
      not try_get_eligible_primes<std::array{7UZ}, 7UZ, 14UZ>().has_value());

  static_assert(
      std::array{11UZ} ==
      try_get_eligible_primes<std::array{7UZ, 11UZ}, 7UZ, 14UZ>().value());
}