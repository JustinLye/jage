#include <jage/engine/time/clock.hpp>
#include <jage/engine/time/durations.hpp>
#include <jage/engine/time/hertz.hpp>

#include <gtest/gtest.h>

using jage::engine::time::operator""_Hz;

TEST(time_clock, Be_able_to_construct_clock) {
  EXPECT_NO_THROW(
      jage::engine::time::clock<jage::engine::time::nanoseconds>{60_Hz});
}
