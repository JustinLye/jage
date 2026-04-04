#include <jage/mp/first_index_of.hpp>
#include <jage/mp/list.hpp>

#include <gtest/gtest.h>

#include <tuple>

using jage::mp::first_index_of;
using jage::mp::list;

struct foo {};
struct bar {};

TEST(jage_mp_first_index_of_test,
     should_return_index_of_first_matching_type_found_in_list) {

  static_assert(0 == first_index_of<list<foo, bar>, foo>);
  static_assert(1 == first_index_of<list<foo, bar>, bar>);
  static_assert(1 == first_index_of<list<foo, bar, bar>, bar>);

  static_assert(
      3 == first_index_of<list<foo, bar, foo, std::tuple<int, double>, bar>,
                          std::tuple<int, double>>);
}

TEST(jage_mp_first_index_of_test,
     should_return_size_of_list_if_matching_type_not_found_in_list) {
  static_assert(1 == first_index_of<list<foo>, bar>);
  static_assert(3 == first_index_of<list<foo, foo, foo>, bar>);
}