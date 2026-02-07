#include <jage/time/clock.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/hertz.hpp>

#include <gtest/gtest.h>

using jage::time::operator""_Hz;

TEST(time_clock, Be_able_to_construct_clock) {
  EXPECT_NO_THROW(jage::time::clock<jage::time::nanoseconds>{60_Hz});
}
