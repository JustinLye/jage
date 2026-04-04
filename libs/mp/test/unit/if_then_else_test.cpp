#include <jage/mp/if_then_else.hpp>

#include <gtest/gtest.h>

#include <concepts>

struct foo {};
struct bar {};

TEST(jage_mp_if_then_else_test, should_be_right) {
  using jage::mp::if_then_else;

  static_assert(std::same_as<foo, if_then_else<true, foo, bar>::type>);
  static_assert(std::same_as<bar, if_then_else<false, foo, bar>::type>);
}