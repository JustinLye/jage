#pragma once

#include <jage/input/keyboard_state.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input {
struct callback_mock {
  MOCK_METHOD(void, call, (const ::jage::input::keyboard_state &), (const));
};

} // namespace jage::test::mocks::input