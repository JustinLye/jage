#pragma once

#include <jage/input/button/states.hpp>
#include <jage/input/keyboard/key.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input::button {
template <class TButton> struct callback {
  MOCK_METHOD(void, call, (const ::jage::input::button::states<TButton> &),
              (const));
};

} // namespace jage::test::mocks::input::button