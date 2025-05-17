#pragma once

#include <jage/input/keyboard/monitor.hpp>

#include <jage/test/mocks/input/callback_mock.hpp>
#include <jage/test/mocks/input/keyboard_driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input {
struct input_keyboard_monitoring : testing::Test {
protected:
  mocks::input::callback_mock mock_callback{};
  mocks::input::keyboard_driver mock_driver{};
  ::jage::input::keyboard::monitor<mocks::input::keyboard_driver> monitor{
      mock_driver};
  using keys = ::jage::input::keyboard::keys;
  static constexpr auto null_callback = [](const auto &) -> void {};

  auto expect_call_to_is_down(const keys key,
                              const std::uint8_t times = 1U) -> void {
    if (times > 0) {
      EXPECT_CALL(mock_driver, is_down(key))
          .Times(times)
          .WillRepeatedly(testing::Return(true));
    } else {
      EXPECT_CALL(mock_driver, is_down(key)).Times(times);
    }
  }

  auto expect_call_to_callback(const std::uint8_t times = 1) -> void {
    EXPECT_CALL(mock_callback, call(testing::_)).Times(times);
  }
};

} // namespace jage::test::fixtures::input