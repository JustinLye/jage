#include <jage/time/durations.hpp>
#include <jage/time/hertz.hpp>

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>

using jage::time::operator""_Hz;

TEST(time_hertz, Convert_to_correct_tick_duration) {
  EXPECT_NEAR(16.6666666667,
              static_cast<jage::time::milliseconds>(60_Hz).count(), 1e-6);
  EXPECT_NEAR(16666.6666666667,
              static_cast<jage::time::microseconds>(60_Hz).count(), 1e-6);
  EXPECT_NEAR(16666666.666666666,
              static_cast<jage::time::nanoseconds>(60_Hz).count(), 1e-6);
  EXPECT_NEAR(33.3333333333,
              static_cast<jage::time::milliseconds>(30_Hz).count(), 1e-6);
  EXPECT_NEAR(33333.3333333333,
              static_cast<jage::time::microseconds>(30_Hz).count(), 1e-6);
  EXPECT_NEAR(33333333.333333332,
              static_cast<jage::time::nanoseconds>(30_Hz).count(), 1e-6);
  EXPECT_NEAR(0.0166666667, static_cast<jage::time::seconds>(60_Hz).count(),
              1e-6);
}

TEST(time_hertz, Throw_on_attempt_to_construct_hertz_with_0) {
  EXPECT_THROW(0_Hz, std::invalid_argument);
}

TEST(time_hertz,
     Throw_on_attempt_to_construct_hertz_with_value_larger_than_hertz_value_type) {
  constexpr auto overflow_value =
      std::numeric_limits<jage::time::hertz::value_type>::max() + 1UZ;
  EXPECT_THROW(jage::time::operator""_Hz(overflow_value),
               std::invalid_argument);

  EXPECT_THROW(jage::time::hertz{overflow_value}, std::invalid_argument);
}
