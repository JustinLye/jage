#pragma once

#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/modifier.hpp>
#include <jage/input/keyboard/scancode.hpp>

#include <bitset>

namespace jage::input::keyboard::events {
struct key_press {
  keyboard::key key;
  keyboard::scancode scancode;
  keyboard::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::input::keyboard::events
