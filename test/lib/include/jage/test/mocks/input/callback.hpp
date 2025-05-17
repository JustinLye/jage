#pragma once

#include <jage/input/button/states.hpp>
#include <jage/input/keyboard/keys.hpp>

#include <gmock/gmock.h>

namespace jage::test::mocks::input {
template <class TButton> struct callback {
  MOCK_METHOD(void, call, (const ::jage::input::button::states<TButton> &),
              (const));
};

} // namespace jage::test::mocks::input