#pragma once

#include <jage/input/keyboard_state.hpp>

#include <gmock/gmock.h>

namespace test::unit::jage::input::mocks {
struct callback_mock {
  MOCK_METHOD(void, call, (const ::jage::input::keyboard_state &), (const));
};

} // namespace test::unit::jage::input::mocks