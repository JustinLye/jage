#pragma once

#include <jage/input/cursor/state.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input::cursor {
struct callback {
  MOCK_METHOD(void, call, (const ::jage::input::cursor::state &), (const));
};
} // namespace jage::test::mocks::input::cursor