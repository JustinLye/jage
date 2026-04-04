#include <jage/mp/contains.hpp>
#include <jage/mp/list.hpp>

#include <gtest/gtest.h>

struct foo {};
struct bar {};
struct baz {};

TEST(jage_mp_contains_test, should_be_true_if_contains) {

  using jage::mp::contains;
  using jage::mp::list;
  static_assert(contains<list<foo, bar>, foo>);
  static_assert(contains<list<foo, bar>, bar>);
  static_assert(not contains<list<foo, bar>, baz>);
}