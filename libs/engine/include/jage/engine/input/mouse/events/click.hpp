#pragma once

#include <jage/engine/input/modifier.hpp>
#include <jage/engine/input/mouse/action.hpp>
#include <jage/engine/input/mouse/button.hpp>

#include <bitset>

namespace jage::engine::input::mouse::events {
struct click {
  mouse::button button;
  mouse::action action;
  std::bitset<modifier_count> modifiers;
};
} // namespace jage::engine::input::mouse::events