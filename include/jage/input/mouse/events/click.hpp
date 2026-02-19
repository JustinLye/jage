#pragma once

#include <jage/input/modifier.hpp>
#include <jage/input/mouse/action.hpp>
#include <jage/input/mouse/button.hpp>

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <bitset>

namespace jage::input::mouse::events {

template <time::internal::concepts::real_number_duration TTimeDuration>
struct click {
  TTimeDuration timestamp;
  mouse::button button;
  mouse::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::input::mouse::events