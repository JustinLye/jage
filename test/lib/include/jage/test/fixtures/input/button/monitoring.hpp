#pragma once

#include <jage/input/keyboard/monitor.hpp>

#include <jage/test/mocks/input/button/callback.hpp>
#include <jage/test/mocks/input/button/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input::button {

template <class TMonitor, class TButton> struct monitoring : testing::Test {
protected:
  mocks::input::button::callback<TButton> mock_callback{};
  mocks::input::button::driver<TButton> mock_driver{};
  TMonitor monitor{mock_driver};
  using button_type = TButton;
  static constexpr auto null_callback = [](const auto &) -> void {};
  static_assert(std::to_underlying(TButton::END) >= 2);
  static constexpr auto button0 =
      static_cast<TButton>(static_cast<std::underlying_type_t<TButton>>(0UZ));

  static constexpr auto button1 =
      static_cast<TButton>(static_cast<std::underlying_type_t<TButton>>(1UZ));
  static constexpr auto button2 =
      static_cast<TButton>(static_cast<std::underlying_type_t<TButton>>(2UZ));

  auto expect_call_to_is_down(const TButton key,
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