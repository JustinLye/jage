#pragma once

#include <jage/time/hertz.hpp>

#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <cmath>
#include <cstdint>
#include <stdexcept>

namespace jage::time::internal {

template <internal::concepts::real_number_time_source TTimeSource> class clock {
  using duration_ = typename TTimeSource::duration;
  duration_ elapsed_time_{};
  duration_ tick_duration_{};
  std::uint64_t elapsed_ticks_{};
  double time_scale_{1.0};

  struct snapshot {
    duration_ real_time{};
    duration_ tick_duration{};
    double time_scale{1.0};
    duration_ elapsed_time{};
    std::uint64_t elapsed_ticks{};
    duration_ game_time{};
    std::uint64_t ticks{};
    duration_ accumulated_time{};
  };

  [[nodiscard]] auto
  ticks(const duration_ current_time) const -> std::uint64_t {
    const auto accumulated_time = current_time - elapsed_time_;
    return std::floor(accumulated_time.count() / tick_duration_.count()) +
           elapsed_ticks_;
  }

public:
  using duration = duration_;

  constexpr clock(const hertz &cycles)
      : tick_duration_{static_cast<duration>(cycles)} {};

  [[nodiscard]] auto real_time() const -> duration {
    return TTimeSource::now().time_since_epoch();
  }

  [[nodiscard]] auto ticks() const -> std::uint64_t {
    return ticks(real_time() * time_scale_);
  }

  [[nodiscard]] auto game_time() const -> duration {
    return duration{ticks() * tick_duration_.count()};
  }

  [[nodiscard]] constexpr auto
  tick_duration() const noexcept -> const duration & {
    return tick_duration_;
  }

  auto set_time_scale(const double scale) -> void {
    if (scale < 0.0) [[unlikely]] {
      throw std::invalid_argument(
          "Refusing to set time scale to a negative value.");
    }
    elapsed_ticks_ = ticks();
    elapsed_time_ = real_time() * scale;

    time_scale_ = scale;
  }

  [[nodiscard]] auto snapshot() const -> snapshot {
    const auto current_real_time = real_time();
    const auto scaled_real_time = current_real_time * time_scale_;
    const auto accumulated_time = scaled_real_time - elapsed_time_;
    const auto accumulated_ticks =
        std::floor(accumulated_time / tick_duration_);
    const auto current_ticks = ticks(scaled_real_time);
    return {
        .real_time = current_real_time,
        .tick_duration = tick_duration_,
        .time_scale = time_scale_,
        .elapsed_time = elapsed_time_,
        .elapsed_ticks = elapsed_ticks_,
        .game_time = duration{current_ticks * tick_duration_.count()},
        .ticks = current_ticks,
        .accumulated_time =
            accumulated_time - accumulated_ticks * tick_duration_,
    };
  }
};

} // namespace jage::time::internal
