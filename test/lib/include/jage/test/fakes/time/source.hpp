#pragma once

#include <jage/time/durations.hpp>

#include <chrono>

namespace jage::test::fakes::time {
template <class TDuration> struct source {
  using rep = typename TDuration::rep;
  using period = typename TDuration::period;
  using duration = TDuration;
  using time_point = std::chrono::time_point<source>;
  static constexpr auto is_steady = false;

  static TDuration current_time;

  static auto now() -> time_point { return time_point{current_time}; }
};

template <class TDuration>
TDuration source<TDuration>::current_time = TDuration{};

static_assert(
    std::chrono::is_clock_v<source<std::chrono::duration<double, std::nano>>>);
static_assert(
    std::chrono::is_clock_v<source<::jage::time::durations::nanoseconds>>);

} // namespace jage::test::fakes::time