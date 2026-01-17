

#include <jage/test/fixtures/game_test.hpp>

#include <GUnit.h>
#include <gmock/gmock.h>

using jage::test::fixtures::game_test;

GTEST(game_test) {

  namespace keyboard = jage::input::keyboard;
  namespace mouse = jage::input::mouse;

  SHOULD("End loop if window is closing") {
    EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
    game.loop();
  }

  SHOULD("Render and poll while window is not closed") {
    testing::InSequence in_sequence{};
    EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(false));
    EXPECT_CALL(window_driver, render()).Times(1);
    EXPECT_CALL(window_driver, poll()).Times(1);
    EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
    game.loop();
  }

  SHOULD("Not poll input controller if window is closed") {
    EXPECT_CALL(window_driver, should_close()).WillOnce(testing::Return(true));
    auto &controller = game.input();
    controller.keyboard().monitor_input(keyboard::keys::escape);
    controller.mouse().monitor_input(mouse::buttons::middle_click);
    std::ignore = controller.cursor().register_callback(null_callback);
    std::ignore = controller.mouse().register_callback(null_callback);
    std::ignore = controller.keyboard().register_callback(null_callback);

    game.loop();
  }

  SHOULD("Poll input controller if window is not closed") {
    EXPECT_CALL(window_driver, poll()).Times(1);
    EXPECT_CALL(window_driver, render()).Times(1);
    EXPECT_CALL(window_driver, should_close())
        .WillOnce(testing::Return(false))
        .WillOnce(testing::Return(true));
    EXPECT_CALL(input_driver, keyboard_is_down(keyboard::keys::escape))
        .Times(1);
    EXPECT_CALL(input_driver, mouse_is_down(mouse::buttons::middle_click))
        .Times(1);
    EXPECT_CALL(input_driver, cursor_position()).Times(1);
    auto &controller = game.input();
    controller.keyboard().monitor_input(keyboard::keys::escape);
    controller.mouse().monitor_input(mouse::buttons::middle_click);
    std::ignore = controller.cursor().register_callback(null_callback);
    std::ignore = controller.mouse().register_callback(null_callback);
    std::ignore = controller.keyboard().register_callback(null_callback);

    game.loop();
  }

  SHOULD("Continue polling when should close is false") {

    EXPECT_CALL(window_driver, should_close())
        .WillOnce(testing::Return(false))
        .WillOnce(testing::Return(false))
        .WillOnce(testing::Return(true));

    game.loop();
  }
}
