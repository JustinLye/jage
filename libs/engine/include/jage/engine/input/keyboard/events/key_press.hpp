#pragma once

#include <jage/engine/input/keyboard/action.hpp>
#include <jage/engine/input/keyboard/key.hpp>
#include <jage/engine/input/keyboard/scancode.hpp>
#include <jage/engine/input/modifier.hpp>

#include <bitset>

namespace jage::engine::input::keyboard::events {
struct key_press {
  keyboard::key key;
  keyboard::scancode scancode;
  keyboard::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::engine::input::keyboard::events
