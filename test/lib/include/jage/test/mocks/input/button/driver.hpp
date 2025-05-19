#pragma once

#include <gmock/gmock.h>

namespace jage::test::mocks::input::button {
template <class TButton> struct driver {
  MOCK_METHOD(bool, is_down, (const TButton), (const));
};
} // namespace jage::test::mocks::input::button