#include <jage/engine/time/durations.hpp>

#include <gtest/gtest.h>

using jage::engine::time::durations::operator""_s;

TEST(time_durations, should_convert_seconds_to_nanoseconds) {
  EXPECT_EQ(1e+9, jage::engine::time::durations::cast<
                      jage::engine::time::durations::nanoseconds>(1_s)
                      .count());
}