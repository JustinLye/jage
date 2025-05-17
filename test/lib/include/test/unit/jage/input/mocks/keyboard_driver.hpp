#pragma once

#include <jage/input/keys.hpp>

#include <gmock/gmock.h>

namespace test::unit::jage::input::mocks {
struct keyboard_driver {
  MOCK_METHOD(bool, is_down, (const ::jage::input::keys));
};
} // namespace test::unit::jage::input::mocks