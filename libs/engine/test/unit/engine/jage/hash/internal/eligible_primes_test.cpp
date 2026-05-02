#include <jage/engine/hash/internal/eligible_primes.hpp>

#include <gtest/gtest.h>

#include <array>

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_select_eligible_primes_for_hashing_finite_set) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(not eligible_primes<std::array{7UZ}, 7UZ, 14UZ>().has_value());

  static_assert(std::array{11UZ} ==
                eligible_primes<std::array{7UZ, 11UZ}, 7UZ, 14UZ>().value());
}