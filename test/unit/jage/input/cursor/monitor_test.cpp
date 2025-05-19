
#include <jage/input/cursor/state.hpp>

#include <jage/test/fixtures/input/cursor/monitoring.hpp>

#include <gtest/gtest.h>

using jage::test::fixtures::input::cursor_monitoring;

TEST_F(cursor_monitoring, should_query_cursor_position) {
  expect_call_to_cursor_position(state);

  std::ignore = monitor.register_callback(null_callback);
  monitor.poll();
}

TEST_F(cursor_monitoring,
       should_not_query_cursor_position_without_registered_callbacks) {
  expect_call_to_cursor_position();
  monitor.poll();
}

TEST_F(cursor_monitoring, should_invoke_registered_callback) {
  expect_call_to_cursor_position(state);
  expect_call_to_callback();

  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        callback.call(state_arg);
      });

  monitor.poll();
}

TEST_F(cursor_monitoring, should_report_correct_cursor_position) {
  state.x_offset = 42;
  state.y_offset = 99;
  expect_call_to_cursor_position(state);

  auto state_output = jage::input::cursor::state{};
  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        state_output = state_arg;
      });
  monitor.poll();

  EXPECT_EQ(state.x_offset, state_output.x_offset);
  EXPECT_EQ(state.y_offset, state_output.y_offset);
}

TEST_F(cursor_monitoring, should_invoke_all_registered_callbacks) {
  expect_call_to_cursor_position(state);
  expect_call_to_callback(2);

  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        this->callback.call(state_arg);
      });
  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        this->callback.call(state_arg);
      });
  this->monitor.poll();
}

TEST_F(cursor_monitoring,
       should_throw_on_attempt_to_register_too_many_callbacks) {
  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        this->callback.call(state_arg);
      });
  std::ignore = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        this->callback.call(state_arg);
      });
  EXPECT_THROW(std::ignore = monitor.register_callback(
                   [&](const jage::input::cursor::state &state_arg) -> void {
                     this->callback.call(state_arg);
                   });
               , std::runtime_error);
}

TEST_F(cursor_monitoring, should_be_able_to_deregister_callback) {
  expect_call_to_cursor_position(state);
  expect_call_to_callback();

  auto registered_callback = monitor.register_callback(
      [&](const jage::input::cursor::state &state_arg) -> void {
        this->callback.call(state_arg);
      });
  this->monitor.poll();
  registered_callback.deregister();
  this->monitor.poll();
}

TEST_F(cursor_monitoring, should_update_after_deregister) {
  expect_call_to_cursor_position(state, 2);
  auto callback0_call_count = 0UZ;
  auto callback1_call_count = 0UZ;

  auto registered_callback = monitor.register_callback(
      [&](const auto &) -> void { ++callback0_call_count; });

  std::ignore = monitor.register_callback(
      [&](const auto &) -> void { ++callback1_call_count; });

  monitor.poll();
  registered_callback.deregister();
  monitor.poll();
  EXPECT_EQ(1UZ, callback0_call_count);
  EXPECT_EQ(2UZ, callback1_call_count);
}
