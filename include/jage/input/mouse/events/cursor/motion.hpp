#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input::mouse::events::cursor {

template <time::internal::concepts::real_number_duration TTimeDuration>
struct motion {
  TTimeDuration timestamp;
  double delta_x;
  double delta_y;
};

} // namespace jage::input::mouse::events::cursor