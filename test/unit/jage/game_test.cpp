

#include <jage/test/fixtures/game_test.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using jage::test::fixtures::game_test;

TEST_F(game_test,
       should_check_if_window_should_not_poll_if_window_should_close_on_loop) {
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  game.loop();
}

TEST_F(game_test, should_render_and_poll_window_while_window_is_not_closed) {
  testing::InSequence in_sequence{};
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(false));
  EXPECT_CALL(window_driver, render()).Times(1);
  EXPECT_CALL(window_driver, poll()).Times(1);
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  game.loop();
}

TEST_F(game_test, should_not_poll_input_controller_if_window_is_closed) {
  EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
  auto &controller = game.input();
  controller.keyboard().monitor_input(jage::input::keyboard::keys::escape);
  controller.mouse().monitor_input(jage::input::mouse::buttons::middle_click);
  std::ignore = controller.cursor().register_callback(null_callback);
  std::ignore = controller.mouse().register_callback(null_callback);
  std::ignore = controller.keyboard().register_callback(null_callback);

  game.loop();
}

TEST_F(game_test, should_poll_input_controller_if_window_is_not_closed) {
  EXPECT_CALL(window_driver, poll()).Times(testing::AnyNumber());
  EXPECT_CALL(window_driver, render()).Times(testing::AnyNumber());
  EXPECT_CALL(window_driver, should_close())
      .Times(2)
      .WillOnce(testing::Return(false))
      .WillOnce(testing::Return(true));
  EXPECT_CALL(input_driver,
              keyboard_is_down(jage::input::keyboard::keys::escape))
      .Times(1);
  EXPECT_CALL(input_driver,
              mouse_is_down(jage::input::mouse::buttons::middle_click))
      .Times(1);
  EXPECT_CALL(input_driver, cursor_position()).Times(1);
  auto &controller = game.input();
  controller.keyboard().monitor_input(jage::input::keyboard::keys::escape);
  controller.mouse().monitor_input(jage::input::mouse::buttons::middle_click);
  std::ignore = controller.cursor().register_callback(null_callback);
  std::ignore = controller.mouse().register_callback(null_callback);
  std::ignore = controller.keyboard().register_callback(null_callback);

  game.loop();
}
