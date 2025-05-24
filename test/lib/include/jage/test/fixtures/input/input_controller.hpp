#pragma once

#include <jage/input/controller.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <jage/test/mocks/input/button/driver.hpp>
#include <jage/test/mocks/input/controller/driver.hpp>
#include <jage/test/mocks/input/cursor/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input {
struct input_controller : public testing::Test {
protected:
  mocks::input::controller::driver driver;
  jage::input::controller<mocks::input::controller::driver> controller{driver};
  static constexpr auto null_callback = [](const auto) -> void {};
};

} // namespace jage::test::fixtures::input