

#include <jage/test/fixtures/game_test.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using jage::test::fixtures::game_test;

namespace keyboard = jage::input::keyboard;
namespace mouse = jage::input::mouse;

TEST_F(game_test, End_loop_if_window_is_closing) {
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  game.loop();
}

TEST_F(game_test, Render_and_poll_while_window_is_not_closed) {
  testing::InSequence in_sequence{};
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(false));
  EXPECT_CALL(window_driver, render()).Times(1);
  EXPECT_CALL(window_driver, poll()).Times(1);
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  game.loop();
}

TEST_F(game_test, Not_poll_input_controller_if_window_is_closed) {
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  auto &input_controller = game.input();
  input_controller.keyboard().monitor_input(keyboard::keys::escape);
  input_controller.mouse().monitor_input(mouse::buttons::middle_click);
  std::ignore = input_controller.cursor().register_callback(null_callback);
  std::ignore = input_controller.mouse().register_callback(null_callback);
  std::ignore = input_controller.keyboard().register_callback(null_callback);

  game.loop();
}

TEST_F(game_test, Poll_input_controller_if_window_is_not_closed) {
  EXPECT_CALL(window_driver, poll()).Times(1);
  EXPECT_CALL(window_driver, render()).Times(1);
  EXPECT_CALL(window_driver, should_close())
      .WillOnce(testing::Return(false))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(input_driver, keyboard_is_down(keyboard::keys::escape)).Times(1);
  EXPECT_CALL(input_driver, mouse_is_down(mouse::buttons::middle_click))
      .Times(1);
  EXPECT_CALL(input_driver, cursor_position()).Times(1);
  auto &input_controller = game.input();
  input_controller.keyboard().monitor_input(keyboard::keys::escape);
  input_controller.mouse().monitor_input(mouse::buttons::middle_click);
  std::ignore = input_controller.cursor().register_callback(null_callback);
  std::ignore = input_controller.mouse().register_callback(null_callback);
  std::ignore = input_controller.keyboard().register_callback(null_callback);

  game.loop();
}

TEST_F(game_test, Continue_polling_when_should_close_is_false) {
  EXPECT_CALL(window_driver, should_close())
      .WillOnce(testing::Return(false))
      .WillOnce(testing::Return(false))
      .WillOnce(testing::Return(true));

  game.loop();
}
