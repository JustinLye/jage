#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input::mouse::events::cursor {
template <time::internal::concepts::real_number_duration TTimeDuration>
struct position {
  TTimeDuration timestamp;
  double x;
  double y;
};
} // namespace jage::input::mouse::events::cursor