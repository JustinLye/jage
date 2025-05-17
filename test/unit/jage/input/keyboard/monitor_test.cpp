#include <jage/input/keyboard/keys.hpp>
#include <jage/input/keyboard/monitor.hpp>
#include <jage/input/keyboard/state.hpp>
#include <jage/input/status.hpp>

#include <jage/test/fixtures/input/input_keyboard_monitoring.hpp>
#include <jage/test/fixtures/input/parameterized_input_keyboard_monitoring.hpp>
#include <jage/test/mocks/input/callback_mock.hpp>
#include <jage/test/mocks/input/keyboard_driver.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace testing;
using jage::test::fixtures::input::input_keyboard_monitoring;
using jage::test::fixtures::input::parameterized_input_keyboard_monitoring;

TEST_F(input_keyboard_monitoring, no_query_if_no_callbacks) {
  expect_call_to_is_down(keys::spacebar, 0U);
  monitor.monitor_input(keys::spacebar);
  monitor.poll();
}

TEST_P(parameterized_input_keyboard_monitoring, should_query_monitor_any_key) {

  const auto key = static_cast<keys>(GetParam());
  expect_call_to_is_down(key);
  monitor.monitor_input(key);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(input_keyboard_monitoring, should_query_all_monitored_keys) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_is_down(keys::escape);

  monitor.monitor_input(keys::spacebar);
  monitor.monitor_input(keys::escape);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(input_keyboard_monitoring, should_allow_variadic_key_input) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_is_down(keys::escape);
  expect_call_to_is_down(keys::a);

  monitor.monitor_input(keys::spacebar, keys::a, keys::escape);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(input_keyboard_monitoring, should_allow_range_input) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_is_down(keys::escape);
  expect_call_to_is_down(keys::a);

  monitor.monitor_input(std::array{
      keys::spacebar,
      keys::a,
      keys::escape,
  });
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(input_keyboard_monitoring, should_invoke_registered_callback) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback();

  monitor.monitor_input(keys::spacebar);
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
}

TEST_F(input_keyboard_monitoring,
       should_report_correct_status_of_monitored_key) {

  expect_call_to_is_down(keys::spacebar);

  monitor.monitor_input(keys::spacebar);
  auto keyboard_state = jage::input::keyboard::state{};
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { keyboard_state = state; });
  monitor.poll();

  EXPECT_TRUE(jage::input::status::down ==
              keyboard_state[keys::spacebar].status);
}

TEST_F(input_keyboard_monitoring, should_invoke_all_registered_callbacks) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback(2);

  monitor.monitor_input(keys::spacebar);
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
}

TEST_F(input_keyboard_monitoring,
       should_throw_on_attempt_to_register_too_many_callbacks) {
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  EXPECT_THROW(std::ignore =
                   monitor.register_callback([&](const auto &state) -> void {
                     mock_callback.call(state);
                   }),
               std::runtime_error);
}

TEST_F(input_keyboard_monitoring, should_be_able_to_deregister_callback) {
  expect_call_to_is_down(keys::spacebar);
  expect_call_to_callback();

  monitor.monitor_input(keys::spacebar);
  auto callback = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
  callback.deregister();
  monitor.poll();
}

TEST_F(input_keyboard_monitoring, should_update_after_deregister) {
  expect_call_to_is_down(keys::spacebar, 2);

  auto callback0_call_count = 0UZ;
  auto callback1_call_count = 0UZ;

  monitor.monitor_input(keys::spacebar);
  auto callback = monitor.register_callback(
      [&](const auto &) -> void { ++callback0_call_count; });
  std::ignore = monitor.register_callback(
      [&](const auto &) -> void { ++callback1_call_count; });
  monitor.poll();
  callback.deregister();
  monitor.poll();
  EXPECT_EQ(1, callback0_call_count);
  EXPECT_EQ(2, callback1_call_count);
}

INSTANTIATE_TEST_SUITE_P(
    , parameterized_input_keyboard_monitoring,
    Range(std::to_underlying(jage::input::keyboard::keys::BEGIN),
          static_cast<std::underlying_type_t<jage::input::keyboard::keys>>(
              std::to_underlying(jage::input::keyboard::keys::END) + 1)));
