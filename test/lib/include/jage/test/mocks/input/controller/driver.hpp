#pragma once

#include <jage/input/keyboard/keys.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <jage/test/mocks/input/button/driver.hpp>
#include <jage/test/mocks/input/cursor/driver.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input::controller {
struct driver : cursor::driver {
  MOCK_METHOD(bool, keyboard_is_down, (const jage::input::keyboard::keys),
              (const));
  MOCK_METHOD(bool, mouse_is_down, (const jage::input::mouse::buttons),
              (const));

  auto is_down(const jage::input::keyboard::keys &key) const -> bool {
    return keyboard_is_down(key);
  }

  auto is_down(const jage::input::mouse::buttons &button) const -> bool {
    return mouse_is_down(button);
  }
};

} // namespace jage::test::mocks::input::controller
