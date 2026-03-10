#include <jage/engine/time/durations.hpp>

#include <jage/engine/time/internal/steady_clock.hpp>

#include <gtest/gtest.h>

using steady_clock =
    jage::engine::time::internal::steady_clock<jage::engine::time::nanoseconds>;

TEST(steady_clock_now, Return_time_point_with_correct_clock_type) {
  const auto time_point = steady_clock::now();
  EXPECT_GT(time_point.time_since_epoch().count(), 0.0);
}
