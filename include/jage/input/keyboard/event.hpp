#pragma once

#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/modifier.hpp>
#include <jage/input/keyboard/scancode.hpp>

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <bitset>

namespace jage::input::keyboard {
template <time::internal::concepts::real_number_duration TDuration>
struct event {
  TDuration timestamp;
  keyboard::key key;
  keyboard::scancode scancode;
  keyboard::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::input::keyboard