#pragma once

#include <jage/input/modifier.hpp>
#include <jage/input/mouse/action.hpp>
#include <jage/input/mouse/button.hpp>

#include <bitset>

namespace jage::input::mouse::events {
struct click {
  mouse::button button;
  mouse::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::input::mouse::events