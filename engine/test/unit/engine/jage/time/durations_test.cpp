#include <jage/time/durations.hpp>

#include <gtest/gtest.h>

using jage::time::durations::operator""_s;

TEST(time_durations, should_convert_seconds_to_nanoseconds) {
  EXPECT_EQ(1e+9,
            jage::time::durations::cast<jage::time::durations::nanoseconds>(1_s)
                .count());
}