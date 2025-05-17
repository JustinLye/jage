#include <jage/input/button/states.hpp>
#include <jage/input/button/status.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/keyboard/monitor.hpp>

#include <jage/test/fixtures/input/keyboard/monitoring.hpp>
#include <jage/test/fixtures/input/keyboard/parameterized_monitoring.hpp>

#include <gtest/gtest.h>

#include <stdexcept>

using namespace testing;
using jage::test::fixtures::input::keyboard::monitoring;
using jage::test::fixtures::input::keyboard::parameterized_monitoring;

TEST_F(monitoring, no_query_if_no_callbacks) {
  expect_call_to_is_down(button_type::spacebar, 0U);
  monitor.monitor_input(button_type::spacebar);
  monitor.poll();
}

TEST_P(parameterized_monitoring, should_query_monitor_any_key) {

  const auto key = static_cast<button_type>(GetParam());
  expect_call_to_is_down(key);
  monitor.monitor_input(key);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(monitoring, should_query_all_monitored_keys) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_is_down(button_type::escape);

  monitor.monitor_input(button_type::spacebar);
  monitor.monitor_input(button_type::escape);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(monitoring, should_allow_variadic_key_input) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_is_down(button_type::escape);
  expect_call_to_is_down(button_type::a);

  monitor.monitor_input(button_type::spacebar, button_type::a,
                        button_type::escape);
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(monitoring, should_allow_range_input) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_is_down(button_type::escape);
  expect_call_to_is_down(button_type::a);

  monitor.monitor_input(std::array{
      button_type::spacebar,
      button_type::a,
      button_type::escape,
  });
  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(monitoring, should_invoke_registered_callback) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_callback();

  monitor.monitor_input(button_type::spacebar);
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
}

TEST_F(monitoring, should_report_correct_status_of_monitored_key) {

  expect_call_to_is_down(button_type::spacebar);

  monitor.monitor_input(button_type::spacebar);
  auto keyboard_state = jage::input::button::states<button_type>{};
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { keyboard_state = state; });
  monitor.poll();

  EXPECT_TRUE(jage::input::button::status::down ==
              keyboard_state[button_type::spacebar].status);
}

TEST_F(monitoring, should_invoke_all_registered_callbacks) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_callback(2);

  monitor.monitor_input(button_type::spacebar);
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  std::ignore = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
}

TEST_F(monitoring, should_throw_on_attempt_to_register_too_many_callbacks) {
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

TEST_F(monitoring, should_be_able_to_deregister_callback) {
  expect_call_to_is_down(button_type::spacebar);
  expect_call_to_callback();

  monitor.monitor_input(button_type::spacebar);
  auto callback = monitor.register_callback(
      [&](const auto &state) -> void { mock_callback.call(state); });
  monitor.poll();
  callback.deregister();
  monitor.poll();
}

TEST_F(monitoring, should_update_after_deregister) {
  expect_call_to_is_down(button_type::spacebar, 2);

  auto callback0_call_count = 0UZ;
  auto callback1_call_count = 0UZ;

  monitor.monitor_input(button_type::spacebar);
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
    , parameterized_monitoring,
    Range(std::to_underlying(jage::input::keyboard::keys::BEGIN),
          static_cast<std::underlying_type_t<jage::input::keyboard::keys>>(
              std::to_underlying(jage::input::keyboard::keys::END) + 1)));
