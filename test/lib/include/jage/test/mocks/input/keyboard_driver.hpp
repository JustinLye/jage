#pragma once

#include <jage/input/keyboard/keys.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input {
struct keyboard_driver {
  MOCK_METHOD(bool, is_down, (const ::jage::input::keyboard::keys));
};
} // namespace jage::test::mocks::input