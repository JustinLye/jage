#pragma once

#include <jage/input/cursor/state.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input::cursor {
struct driver {
  MOCK_METHOD(::jage::input::cursor::state, cursor_position, (), (const));
};

} // namespace jage::test::mocks::input::cursor