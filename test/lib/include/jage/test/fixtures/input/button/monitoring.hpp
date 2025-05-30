#pragma once

#include <jage/input/keyboard/monitor.hpp>

#include <jage/test/mocks/input/button/callback.hpp>
#include <jage/test/mocks/input/button/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input::button {

template <class TMonitor> struct monitoring : testing::Test {
protected:
  using button_type_t = typename TMonitor::monitor_target_t;
  mocks::input::button::callback<button_type_t> mock_callback{};
  mocks::input::button::driver<button_type_t> mock_driver{};
  TMonitor monitor{mock_driver};
  using button_type = button_type_t;
  static constexpr auto null_callback = [](const auto &) -> void {};
  static_assert(std::to_underlying(button_type_t::END) >= 2);
  static constexpr auto button0 = static_cast<button_type_t>(
      static_cast<std::underlying_type_t<button_type_t>>(0UZ));

  static constexpr auto button1 = static_cast<button_type_t>(
      static_cast<std::underlying_type_t<button_type_t>>(1UZ));
  static constexpr auto button2 = static_cast<button_type_t>(
      static_cast<std::underlying_type_t<button_type_t>>(2UZ));

  auto expect_call_to_is_down(const button_type_t key,
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

} // namespace jage::test::fixtures::input::button