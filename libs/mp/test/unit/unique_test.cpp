#include <jage/mp/list.hpp>
#include <jage/mp/unique.hpp>

#include <gtest/gtest.h>

#include <concepts>

struct foo {};
struct bar {};
struct baz {};

TEST(jage_mp_unqiue_test, should_be_unique) {

  using jage::mp::list;
  using jage::mp::unique;

  static_assert(std::same_as<list<foo, bar>, unique<list<foo, bar>>::type>);
  static_assert(std::same_as<list<>, unique<list<>>::type>);

  static_assert(std::same_as<list<foo>, unique<list<foo, foo>>::type>);
  static_assert(
      std::same_as<list<foo, bar>, unique<list<foo, bar, foo>>::type>);
  static_assert(std::same_as<list<foo, bar, baz>,
                             unique<list<foo, foo, bar, bar, foo, baz, bar, foo,
                                         baz, foo, bar, bar>>::type>);

  //   static_assert(std::same_as<list<foo>, unique<list<foo, foo>>::type>);
}