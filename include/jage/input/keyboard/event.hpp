#pragma once

#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/modifier.hpp>
#include <jage/input/keyboard/scan_code.hpp>

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <bitset>

namespace jage::input::keyboard {
template <time::internal::concepts::real_number_duration TDuration>
struct event {
  TDuration timestamp;
  keyboard::key key;
  keyboard::scan_code scan_code;
  keyboard::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::input::keyboard