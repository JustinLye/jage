#pragma once

#include <jage/time/hertz.hpp>

#include <jage/time/internal/concepts/real_number_time_source.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace jage::time::internal {

template <internal::concepts::real_number_time_source TTimeSource> class clock {
  using duration_ = typename TTimeSource::duration;
  duration_ tick_duration_;
  double time_scale_{1.0};
  duration_ elapsed_time_{};
  std::uint64_t elapsed_ticks_{};

  [[nodiscard]] auto
  ticks(const duration_ time_elapsed) const -> std::uint64_t {
    return std::floor(time_elapsed.count() / tick_duration_.count());
  }

public:
  using duration = duration_;

  constexpr clock(const hertz &cycles)
      : tick_duration_{static_cast<duration>(cycles)} {};

  [[nodiscard]] auto real_time() const -> duration {
    return TTimeSource::now().time_since_epoch();
  }

  [[nodiscard]] auto ticks() const -> std::uint64_t {
    const auto current_time = real_time() * time_scale_;
    const auto accumulated_time =
        std::max(duration{0}, (current_time - elapsed_time_));
    return ticks(accumulated_time) + elapsed_ticks_;
  }

  [[nodiscard]] auto game_time() const -> duration {
    return duration{ticks() * tick_duration_.count()};
  }

  [[nodiscard]] constexpr auto
  tick_duration() const noexcept -> const duration & {
    return tick_duration_;
  }

  auto set_time_scale(const double scale) -> void {
    elapsed_ticks_ = ticks();
    elapsed_time_ = real_time() * scale;

    time_scale_ = scale;
  }
};

} // namespace jage::time::internal