#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input::mouse::events {

template <time::internal::concepts::real_number_duration TTimeDuration>
struct horizontal_scroll {
  TTimeDuration timestamp;
  double offset;
};
} // namespace jage::input::mouse::events