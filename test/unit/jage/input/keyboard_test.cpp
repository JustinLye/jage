#include <jage/input/key_status.hpp>
#include <jage/input/keyboard.hpp>
#include <jage/input/keyboard_state.hpp>
#include <jage/input/keys.hpp>

#include <jage/test/mocks/input/callback_mock.hpp>
#include <jage/test/mocks/input/keyboard_driver.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace jage::test;
using namespace testing;

struct input_keyboard_monitoring : Test {
protected:
  mocks::input::callback_mock mock_callback{};
  mocks::input::keyboard_driver mock_driver{};
  jage::input::keyboard<mocks::input::keyboard_driver> keyboard{mock_driver};
  static constexpr auto null_callback = [](const auto &) -> void {};

  auto expect_call_to_is_down(const jage::input::keys key,
                              const std::uint8_t times = 1U) -> void {
    if (times > 0) {
      EXPECT_CALL(mock_driver, is_down(key))
          .Times(times)
          .WillRepeatedly(Return(true));
    } else {
      EXPECT_CALL(mock_driver, is_down(key)).Times(times);
    }
  }

  auto expect_call_to_callback(const std::uint8_t times = 1) -> void {
    EXPECT_CALL(mock_callback, call(_)).Times(times);
  }
};

struct parameterized_input_keyboard_monitoring
    : input_keyboard_monitoring,
      WithParamInterface<std::underlying_type_t<jage::input::keys>> {};

TEST_F(input_keyboard_monitoring, no_query_if_no_callbacks) {
  expect_call_to_is_down(jage::input::keys::spacebar, 0U);
  keyboard.monitor_input(jage::input::keys::spacebar);
  keyboard.poll();
}

TEST_P(parameterized_input_keyboard_monitoring, should_query_monitor_any_key) {

  const auto key = static_cast<jage::input::keys>(GetParam());
  expect_call_to_is_down(key);
  keyboard.monitor_input(key);
  std::ignore = keyboard.register_callback(null_callback);
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring, should_query_all_monitored_keys) {
  expect_call_to_is_down(jage::input::keys::spacebar);
  expect_call_to_is_down(jage::input::keys::escape);

  keyboard.monitor_input(jage::input::keys::spacebar);
  keyboard.monitor_input(jage::input::keys::escape);
  std::ignore = keyboard.register_callback(null_callback);
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring, should_allow_variadic_key_input) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_is_down(keys::escape);
  expect_call_to_is_down(keys::a);

  keyboard.monitor_input(keys::spacebar, keys::a, keys::escape);
  std::ignore = keyboard.register_callback(null_callback);
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring, should_allow_range_input) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_is_down(keys::escape);
  expect_call_to_is_down(keys::a);

  keyboard.monitor_input(std::array{
      keys::spacebar,
      keys::a,
      keys::escape,
  });
  std::ignore = keyboard.register_callback(null_callback);
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring, should_invoke_registered_callback) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback();

  keyboard.monitor_input(keys::spacebar);
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring,
       should_report_correct_status_of_monitored_key) {
  using jage::input::keys;

  expect_call_to_is_down(keys::spacebar);

  keyboard.monitor_input(keys::spacebar);
  auto keyboard_state = jage::input::keyboard_state{};
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { keyboard_state = state; });
  keyboard.poll();

  EXPECT_TRUE(jage::input::key_status::down ==
              keyboard_state[keys::spacebar].status);
}

TEST_F(input_keyboard_monitoring, should_invoke_all_registered_callbacks) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback(2);

  keyboard.monitor_input(keys::spacebar);
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring,
       should_throw_on_attempt_to_register_too_many_callbacks) {
  using jage::input::keys;
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  std::ignore = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  EXPECT_THROW(std::ignore =
                   keyboard.register_callback([&](const auto &state) -> void {
                     mock_callback.call(state);
                   }),
               std::runtime_error);
}

TEST_F(input_keyboard_monitoring, should_be_able_to_deregister_callback) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback();

  keyboard.monitor_input(keys::spacebar);
  auto callback = keyboard.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  keyboard.poll();
  callback.deregister();
  keyboard.poll();
}

TEST_F(input_keyboard_monitoring, should_update_after_deregister) {
  using jage::input::keys;
  expect_call_to_is_down(keys::spacebar, 2);

  auto callback0_call_count = 0UZ;
  auto callback1_call_count = 0UZ;

  keyboard.monitor_input(keys::spacebar);
  auto callback = keyboard.register_callback(
      [&](const auto &) -> void { ++callback0_call_count; });
  std::ignore = keyboard.register_callback(
      [&](const auto &) -> void { ++callback1_call_count; });
  keyboard.poll();
  callback.deregister();
  keyboard.poll();
  EXPECT_EQ(1, callback0_call_count);
  EXPECT_EQ(2, callback1_call_count);
}

INSTANTIATE_TEST_SUITE_P(
    , parameterized_input_keyboard_monitoring,
    Range(std::to_underlying(jage::input::keys::BEGIN),
          static_cast<std::underlying_type_t<jage::input::keys>>(
              std::to_underlying(jage::input::keys::END) + 1)));
