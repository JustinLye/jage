#include <jage/mp/max_value.hpp>

#include <gtest/gtest.h>

TEST(jage_mp_max_value, should_find_max_value) {

  using jage::mp::max_value;

  static_assert(0 == max_value<>);
  static_assert(1 == max_value<1>);
  static_assert(400 == max_value<100, 400, 101>);
  static_assert(-100 == max_value<-5000, -100, -400, -101>);
  static_assert(65535UZ ==
                max_value<100, 200, 300, -400, 65535UZ, 200U, 500, 800>);
}