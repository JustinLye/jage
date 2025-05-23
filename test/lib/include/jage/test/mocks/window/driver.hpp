#pragma once

#include <gmock/gmock.h>

namespace jage::test::mocks::window {
struct driver {
  MOCK_METHOD(bool, should_close, (), (const));
  MOCK_METHOD(void, poll, ());
};
} // namespace jage::test::mocks::window