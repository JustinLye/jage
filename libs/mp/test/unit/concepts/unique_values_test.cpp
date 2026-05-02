#include <jage/mp/concepts/unique_values.hpp>

#include <gtest/gtest.h>

TEST(jage_mp_concepts_unique_values, should_be_unique) {

  using jage::mp::concepts::unique_values;

  static_assert(unique_values<>);
  static_assert(unique_values<1>);
  static_assert(unique_values<100, 400, 101>);
  static_assert(not unique_values<100, 200, 300, 400, 200, 500, 800>);
}