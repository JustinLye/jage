#include <jage/time/durations.hpp>
#include <jage/time/hertz.hpp>

#include <jage/time/internal/clock.hpp>

#include <jage/test/fakes/time/source.hpp>

#include <GUnit.h>

#include <chrono>
#include <stdexcept>

GTEST("clock: queries") {
  using time_source = jage::test::fakes::time::source<jage::time::nanoseconds>;
  using jage::time::operator""_ns;
  using jage::time::operator""_Hz;

  auto &current_time = time_source::current_time;
  current_time = 42_ns;

  auto clock = jage::time::internal::clock<time_source>{60_Hz};

  SHOULD("Return real-time") { EXPECT_EQ(42_ns, clock.real_time()); }

  SHOULD("Return correct tick duration") {
    EXPECT_NEAR(16666666.666666666, clock.tick_duration().count(), 1e-6);
    EXPECT_NEAR(
        33333333.333333332,
        jage::time::internal::clock<time_source>{30_Hz}.tick_duration().count(),
        1e-6);
  }

  SHOULD("Return game-time") {
    const auto &tick_duration = clock.tick_duration();
    current_time = tick_duration - 1_ns;

    EXPECT_NEAR((0_ns).count(), clock.game_time().count(), 1e-6);

    current_time += 1_ns;
    EXPECT_NEAR(tick_duration.count(), clock.game_time().count(), 1e-6);

    current_time += tick_duration;
    EXPECT_NEAR(tick_duration.count() * 2, clock.game_time().count(), 1e-6);
  }

  SHOULD("Return correct-ticks") {
    const auto &tick_duration = clock.tick_duration();

    current_time = tick_duration - 1_ns;
    EXPECT_EQ(0UZ, clock.ticks());

    current_time += 1_ns;
    EXPECT_EQ(1UZ, clock.ticks());

    current_time += tick_duration;
    EXPECT_EQ(2UZ, clock.ticks());
  }

  SHOULD("Use time_source duration") {
    {
      const auto clock = jage::time::internal::clock<
          jage::test::fakes::time::source<jage::time::microseconds>>{30_Hz};
      EXPECT_NEAR(33333.3333333333, clock.tick_duration().count(), 1e-6);
    }
    {
      const auto clock = jage::time::internal::clock<
          jage::test::fakes::time::source<jage::time::milliseconds>>{60_Hz};
      EXPECT_NEAR(16.6666666667, clock.tick_duration().count(), 1e-6);
    }
  }
  SHOULD("Be stable") {
    {
      auto clock = jage::time::internal::clock<time_source>{200_Hz};
      current_time = 0_ns;
      EXPECT_EQ(0UZ, clock.ticks());
      EXPECT_EQ(0UZ, clock.ticks());
      current_time = 5000000_ns;
      EXPECT_EQ(1UZ, clock.ticks());
      EXPECT_EQ(1UZ, clock.ticks());
      current_time = 15000000_ns;
      EXPECT_EQ(3UZ, clock.ticks());
      EXPECT_EQ(3UZ, clock.ticks());
    }
  }
}

GTEST("clock: scale") {
  using time_source = jage::test::fakes::time::source<jage::time::nanoseconds>;
  using jage::time::operator""_ns;
  using jage::time::operator""_Hz;

  auto clock = jage::time::internal::clock<time_source>{60_Hz};

  auto &current_time = time_source::current_time;
  current_time = 17000000_ns;

  ASSERT_EQ(1UZ, clock.ticks());
  ASSERT_NEAR(16666666.666666666, clock.game_time().count(), 1e-4);
  ASSERT_NEAR(17000000.0, clock.real_time().count(), 1e-6);

  SHOULD("reject negative time scale") {
    EXPECT_THROW(clock.set_time_scale(-1.0), std::invalid_argument);
  }

  clock.set_time_scale(0);
  current_time += 50000000_ns;
  ASSERT_NEAR(67000000.0, clock.real_time().count(), 1e-6);

  SHOULD("Freeze ticks when paused") { EXPECT_EQ(1UZ, clock.ticks()); }

  SHOULD("Freeze game time when paused") {
    EXPECT_NEAR(16666666.666666666, clock.game_time().count(), 1e-6);
  }

  clock.set_time_scale(1);

  SHOULD("have same tick after resuming") { EXPECT_EQ(1UZ, clock.ticks()); }

  SHOULD("have same game time after resuming") {
    EXPECT_NEAR(16666666.666666666, clock.game_time().count(), 1e-6);
  }

  current_time += 17000000_ns;

  SHOULD("tick in sync with time scale") { EXPECT_EQ(2UZ, clock.ticks()); }

  clock.set_time_scale(2);

  SHOULD("have same number of ticks after doubling time scale") {
    EXPECT_EQ(2UZ, clock.ticks());
  }

  current_time += 17000000_ns;

  SHOULD("tick x2") { EXPECT_EQ(4UZ, clock.ticks()); }

  clock.set_time_scale(0.5);

  SHOULD("have same number of ticks after slowing time scale") {
    EXPECT_EQ(4UZ, clock.ticks());
  }

  current_time += 34000000_ns;

  SHOULD("tick at half speed") { EXPECT_EQ(5UZ, clock.ticks()); }

  clock.set_time_scale(0);

  SHOULD("freeze on time-scale of 0") { EXPECT_EQ(5UZ, clock.ticks()); }

  current_time += 34000000_ns;
  SHOULD("not update ticks when frozen after pausing once before") {
    EXPECT_EQ(5UZ, clock.ticks());
  }

  clock.set_time_scale(10);
  SHOULD("have same number of ticks after unpausing at scale-up time") {
    EXPECT_EQ(5UZ, clock.ticks());
  }

  current_time += 1700000_ns;

  SHOULD("tick at x10") { EXPECT_EQ(6UZ, clock.ticks()); }

  clock.set_time_scale(0);
  current_time += 17000000_ns;
  ASSERT_EQ(6UZ, clock.ticks());

  clock.set_time_scale(0.10);
  SHOULD("have same number of ticks after unpausing with scaled-down time") {
    EXPECT_EQ(6UZ, clock.ticks());
  }

  current_time += 170'000'000_ns;
  SHOULD("tick at 1/10") { EXPECT_EQ(7UZ, clock.ticks()); }
}

GTEST("clock: snapshot") {

  using time_source = jage::test::fakes::time::source<jage::time::nanoseconds>;
  using jage::time::operator""_ns;
  using jage::time::operator""_ms;
  using jage::time::operator""_Hz;

  auto clock = jage::time::internal::clock<time_source>{10000_Hz};

  auto &current_time = time_source::current_time;
  current_time = 0_ns;

  SHOULD("should have zero calculated values") {
    const auto snapshot = clock.snapshot();

    EXPECT_EQ(0_ns, snapshot.real_time);
    EXPECT_EQ(100'000_ns, snapshot.tick_duration);
    EXPECT_EQ(1.0, snapshot.time_scale);
    EXPECT_EQ(0UZ, snapshot.frame);
    EXPECT_EQ(0UZ, snapshot.elapsed_frames);
    EXPECT_EQ(0_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  SHOULD("update tick duration based on hertz") {
    const auto local_clock = jage::time::internal::clock<
        jage::test::fakes::time::source<jage::time::milliseconds>>{100_Hz};
    EXPECT_EQ(10_ms, local_clock.snapshot().tick_duration);
  }

  current_time = 100'000_ns;

  SHOULD("update snapshot with current time") {
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

  SHOULD("update time scale") {
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

  SHOULD("use elapsed_time for accumulated_time") {
    const auto snapshot = clock.snapshot();

    EXPECT_EQ(150'000_ns, snapshot.real_time);
    EXPECT_EQ(2.0, snapshot.time_scale);
    EXPECT_EQ(2UZ, snapshot.frame);
    EXPECT_EQ(1UZ, snapshot.elapsed_frames);
    EXPECT_EQ(200'000_ns, snapshot.elapsed_time);
    EXPECT_EQ(0_ns, snapshot.accumulated_time);
  }

  current_time += 50'000_ns;

  SHOULD("scale snapshot") {
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

  SHOULD("freeze appropriate parts of snapshot") {
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

  SHOULD("unfreeze") {
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

  SHOULD("continue updating snapshot") {
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
  SHOULD("freeze accumulated time") {
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
