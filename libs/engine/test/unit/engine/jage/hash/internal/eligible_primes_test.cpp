#include <jage/engine/hash/internal/eligible_primes.hpp>

#include <gtest/gtest.h>

#include <array>
#include <concepts>

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_select_eligible_primes_for_various_unsigned_inputs) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(std::array<std::uint64_t, 0>{} ==
                eligible_primes<std::array{7UZ}, 7UZ, 14UZ>);

  static_assert(std::array<std::uint64_t, 0>{} ==
                eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 5UZ, 5UZ>);

  static_assert(std::array{11UZ} ==
                eligible_primes<std::array{7UZ, 11UZ}, 7UZ, 14UZ>);

  static_assert(std::array{13UZ} ==
                eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 1UZ, 78UZ>);

  static_assert(std::array{11UZ, 13UZ} ==
                eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 7UZ, 14UZ>);

  static_assert(std::array{7UZ, 11UZ, 13UZ} ==
                eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 42UZ>);

  static_assert(std::array<std::uint64_t, 0>{} ==
                eligible_primes<std::array{7UZ, 11UZ}, 0UZ, 0UZ>);
}

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_preserve_prime_order_when_multiple_are_eligible) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(std::array{11UZ, 17UZ} ==
                eligible_primes<std::array{7UZ, 11UZ, 17UZ}, 7UZ, 14UZ>);
}

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_deduce_result_type_for_unsigned_inputs) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(
      std::same_as<decltype(eligible_primes<std::array{7UZ, 11UZ}, 7UZ, 14UZ>),
                   const std::array<std::uint64_t, 1>>);
}

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_deduce_result_type_for_signed_inputs) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(std::array<std::int32_t, 2>{11, 13} ==
                eligible_primes<std::array{7, 11, 13}, 7, 14>);

  static_assert(
      std::same_as<decltype(eligible_primes<std::array{7, 11, 13}, 7, 14>),
                   const std::array<std::int32_t, 2>>);
}

TEST(jage_engine_hash_internal_eligible_primes_test,
     should_deduce_value_type_from_primes_instead_of_values) {

  using jage::engine::hash::internal::eligible_primes;

  static_assert(std::array<std::uint64_t, 2>{11UZ, 13UZ} ==
                eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 7, 14UZ>);

  static_assert(std::same_as<
                decltype(eligible_primes<std::array{7UZ, 11UZ, 13UZ}, 7, 14UZ>),
                const std::array<std::uint64_t, 2>>);
}
