#pragma once

#include <jage/input/cursor/monitor.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/monitor.hpp>
#include <jage/input/mouse/buttons.hpp>
#include <jage/input/mouse/monitor.hpp>

namespace jage::input {
template <class TDriver> class controller {
  keyboard::monitor<TDriver> keyboard_;
  mouse::monitor<TDriver> mouse_;
  cursor::monitor<TDriver> cursor_;

public:
  explicit controller(TDriver &driver)
      : keyboard_{driver}, mouse_{driver}, cursor_{driver} {}
  auto keyboard() -> keyboard::monitor<TDriver> & { return keyboard_; }
  auto mouse() -> mouse::monitor<TDriver> & { return mouse_; }
  auto cursor() -> cursor::monitor<TDriver> & { return cursor_; }
  auto poll() -> void {
    keyboard_.poll();
    mouse_.poll();
    cursor_.poll();
  }
};
} // namespace jage::input