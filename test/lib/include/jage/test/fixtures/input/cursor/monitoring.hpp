#pragma once

#include <jage/input/cursor/monitor.hpp>
#include <jage/input/cursor/state.hpp>

#include <jage/test/mocks/input/cursor/callback.hpp>
#include <jage/test/mocks/input/cursor/driver.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace jage::test::fixtures::input {
struct cursor_monitoring : testing::Test {
protected:
  static constexpr auto null_callback = [](const auto &) -> void {};
  ::jage::input::cursor::state state{};
  mocks::input::cursor::driver driver{};
  mocks::input::cursor::callback callback{};
  ::jage::input::cursor::monitor<mocks::input::cursor::driver> monitor{driver};
  auto expect_call_to_cursor_position(const ::jage::input::cursor::state &state,
                                      const std::uint8_t times = 1) -> void {
    EXPECT_CALL(driver, cursor_position())
        .Times(times)
        .WillRepeatedly(testing::Return(state));
  }
  auto expect_call_to_cursor_position() -> void {
    EXPECT_CALL(driver, cursor_position).Times(0);
  }
  auto expect_call_to_callback(const std::uint8_t times = 1) -> void {
    EXPECT_CALL(callback, call(testing::_)).Times(times);
  }
};

} // namespace jage::test::fixtures::input
