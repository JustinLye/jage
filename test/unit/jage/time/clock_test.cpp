#include <jage/time/clock.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/hertz.hpp>

#include <GUnit.h>

GTEST("time clock") {
  using jage::time::operator""_Hz;

  SHOULD("be able to construct clock") {
    EXPECT_NO_THROW(jage::time::clock<jage::time::nanoseconds>{60_Hz});
  }
}