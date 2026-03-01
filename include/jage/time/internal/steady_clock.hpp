#pragma once

#include <jage/time/durations.hpp>

#include <chrono>

namespace jage::time::internal {
template <class TDuration> struct steady_clock {
  using rep = typename TDuration::rep;
  using period = typename TDuration::period;
  using duration = TDuration;
  using time_point = std::chrono::time_point<steady_clock>;
  static constexpr auto is_steady = true;

  [[nodiscard]] static auto now() -> time_point {
    return time_point{
        std::chrono::time_point_cast<duration>(std::chrono::steady_clock::now())
            .time_since_epoch()};
  }
};

static_assert(std::chrono::is_clock_v<steady_clock<nanoseconds>>);
} // namespace jage::time::internal