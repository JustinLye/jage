#include <jage/time/durations.hpp>
#include <jage/time/hertz.hpp>

#include <jage/time/internal/clock.hpp>

#include <jage/test/fakes/time/source.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

using time_source = jage::test::fakes::time::source<jage::time::nanoseconds>;
using jage::time::operator""_ns;
using jage::time::operator""_Hz;

class clock_queries : public ::testing::Test {
protected:
  jage::time::internal::clock<time_source> clock{60_Hz};

  auto SetUp() -> void override { time_source::current_time = 42_ns; }
};

TEST_F(clock_queries, Return_real_time) { EXPECT_EQ(42_ns, clock.real_time()); }

TEST_F(clock_queries, Return_correct_tick_duration) {
  EXPECT_NEAR(16666666.666666666, clock.tick_duration().count(), 1e-6);
  EXPECT_NEAR(
      33333333.333333332,
      jage::time::internal::clock<time_source>{30_Hz}.tick_duration().count(),
      1e-6);
}

TEST_F(clock_queries, Return_game_time) {
  auto &current_time = time_source::current_time;
  const auto &tick_duration = clock.tick_duration();
  current_time = tick_duration - 1_ns;

  EXPECT_NEAR((0_ns).count(), clock.game_time().count(), 1e-6);

  current_time += 1_ns;
  EXPECT_NEAR(tick_duration.count(), clock.game_time().count(), 1e-6);

  current_time += tick_duration;
  EXPECT_NEAR(tick_duration.count() * 2, clock.game_time().count(), 1e-6);
}

TEST_F(clock_queries, Return_correct_ticks) {
  auto &current_time = time_source::current_time;
  const auto &tick_duration = clock.tick_duration();

  current_time = tick_duration - 1_ns;
  EXPECT_EQ(0UZ, clock.ticks());

  current_time += 1_ns;
  EXPECT_EQ(1UZ, clock.ticks());

  current_time += tick_duration;
  EXPECT_EQ(2UZ, clock.ticks());
}

TEST_F(clock_queries, Use_time_source_duration) {
  {
    const auto sut = jage::time::internal::clock<
        jage::test::fakes::time::source<jage::time::microseconds>>{30_Hz};
    EXPECT_NEAR(33333.3333333333, sut.tick_duration().count(), 1e-6);
  }
  {
    const auto sut = jage::time::internal::clock<
        jage::test::fakes::time::source<jage::time::milliseconds>>{60_Hz};
    EXPECT_NEAR(16.6666666667, sut.tick_duration().count(), 1e-6);
  }
}

TEST_F(clock_queries, Be_stable) {
  auto &current_time = time_source::current_time;
  auto sut = jage::time::internal::clock<time_source>{200_Hz};
  current_time = 0_ns;
  EXPECT_EQ(0UZ, sut.ticks());
  EXPECT_EQ(0UZ, sut.ticks());
  current_time = 5000000_ns;
  EXPECT_EQ(1UZ, sut.ticks());
  EXPECT_EQ(1UZ, sut.ticks());
  current_time = 15000000_ns;
  EXPECT_EQ(3UZ, sut.ticks());
  EXPECT_EQ(3UZ, sut.ticks());
}

TEST(clock_scale, Time_scale_operations) {
  auto &current_time = time_source::current_time;
  auto clock = jage::time::internal::clock<time_source>{60_Hz};
  current_time = 17000000_ns;

  ASSERT_EQ(1UZ, clock.ticks());
  ASSERT_NEAR(16666666.666666666, clock.game_time().count(), 1e-4);
  ASSERT_NEAR(17000000.0, clock.real_time().count(), 1e-6);

  EXPECT_THROW(clock.set_time_scale(-1.0), std::invalid_argument);

  clock.set_time_scale(0);
  current_time += 50000000_ns;
  ASSERT_NEAR(67000000.0, clock.real_time().count(), 1e-6);

  EXPECT_EQ(1UZ, clock.ticks());
  EXPECT_NEAR(16666666.666666666, clock.game_time().count(), 1e-6);

  clock.set_time_scale(1);

  EXPECT_EQ(1UZ, clock.ticks());
  EXPECT_NEAR(16666666.666666666, clock.game_time().count(), 1e-6);

  current_time += 17000000_ns;

  EXPECT_EQ(2UZ, clock.ticks());

  clock.set_time_scale(2);

  EXPECT_EQ(2UZ, clock.ticks());

  current_time += 17000000_ns;

  EXPECT_EQ(4UZ, clock.ticks());

  clock.set_time_scale(0.5);

  EXPECT_EQ(4UZ, clock.ticks());

  current_time += 34000000_ns;

  EXPECT_EQ(5UZ, clock.ticks());

  clock.set_time_scale(0);

  EXPECT_EQ(5UZ, clock.ticks());

  current_time += 34000000_ns;
  EXPECT_EQ(5UZ, clock.ticks());

  clock.set_time_scale(10);
  EXPECT_EQ(5UZ, clock.ticks());

  current_time += 1700000_ns;

  EXPECT_EQ(6UZ, clock.ticks());

  clock.set_time_scale(0);
  current_time += 17000000_ns;
  ASSERT_EQ(6UZ, clock.ticks());

  clock.set_time_scale(0.10);
  EXPECT_EQ(6UZ, clock.ticks());

  current_time += 170'000'000_ns;
  EXPECT_EQ(7UZ, clock.ticks());
}

TEST(clock_snapshot, Snapshot_reporting) {
  using jage::time::operator""_ms;

  auto clock = jage::time::internal::clock<time_source>{10000_Hz};
  auto &current_time = time_source::current_time;
  current_time = 0_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(0_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(1.0, snapshot.time_scale);
    EXPECT_EQ(0UZ, snapshot.frame);
    EXPECT_EQ(0UZ, snapshot.elapsed_frames);
    EXPECT_EQ(0_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  {
    const auto local_clock = jage::time::internal::clock<
        jage::test::fakes::time::source<jage::time::milliseconds>>{100_Hz};
    EXPECT_EQ(10_ms, local_clock.snapshot().tick_duration);
  }

  current_time = 100'000_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(100'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(1.0, snapshot.time_scale);
    EXPECT_EQ(1UZ, snapshot.frame);
    EXPECT_EQ(0UZ, snapshot.elapsed_frames);
    EXPECT_EQ(0_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  clock.set_time_scale(2.0);

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(100'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(2.0, snapshot.time_scale);
    EXPECT_EQ(1UZ, snapshot.frame);
    EXPECT_EQ(1UZ, snapshot.elapsed_frames);
    EXPECT_EQ(200'000_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  current_time += 50'000_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(150'000_ns, snapshot.real_time);
    EXPECT_EQ(2.0, snapshot.time_scale);
    EXPECT_EQ(2UZ, snapshot.frame);
    EXPECT_EQ(1UZ, snapshot.elapsed_frames);
    EXPECT_EQ(200'000_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  current_time += 50'000_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(200'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(2.0, snapshot.time_scale);
    EXPECT_EQ(3UZ, snapshot.frame);
    EXPECT_EQ(1UZ, snapshot.elapsed_frames);
    EXPECT_EQ(200'000_ns, snapshot.elapsed_time);
  }

  clock.set_time_scale(0);
  current_time += 100'000_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(current_time, snapshot.real_time);
    EXPECT_EQ(300'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(0.0, snapshot.time_scale);
    EXPECT_EQ(3UZ, snapshot.frame);
    EXPECT_EQ(3UZ, snapshot.elapsed_frames);
    EXPECT_EQ(0_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  clock.set_time_scale(1);

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(current_time, snapshot.real_time);
    EXPECT_EQ(300'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(1.0, snapshot.time_scale);
    EXPECT_EQ(3UZ, snapshot.frame);
    EXPECT_EQ(3UZ, snapshot.elapsed_frames);
    EXPECT_EQ(300'000_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  current_time += 150'000_ns;

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(current_time, snapshot.real_time);
    EXPECT_EQ(450'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(1.0, snapshot.time_scale);
    EXPECT_EQ(4UZ, snapshot.frame);
    EXPECT_EQ(3UZ, snapshot.elapsed_frames);
    EXPECT_EQ(300'000_ns, snapshot.elapsed_time);
    EXPECT_EQ(50'000_ns, snapshot.accumulated_time);
  }

  clock.set_time_scale(0);

  {
    const auto snapshot = clock.snapshot();
    EXPECT_EQ(current_time, snapshot.real_time);
    EXPECT_EQ(450'000_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(0.0, snapshot.time_scale);
    EXPECT_EQ(4UZ, snapshot.frame);
    EXPECT_EQ(4UZ, snapshot.elapsed_frames);
    EXPECT_EQ(0_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }
}
