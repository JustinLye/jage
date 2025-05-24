
#include <jage/input/controller.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <jage/test/fixtures/input/input_controller.hpp>

#include <gtest/gtest.h>

using jage::test::fixtures::input::input_controller;

TEST_F(input_controller, should_check_button_down) {
  std::ignore = controller.keyboard().register_callback(null_callback);
  controller.keyboard().monitor_input(jage::input::keyboard::keys::a);

  EXPECT_CALL(driver, keyboard_is_down(jage::input::keyboard::keys::a))
      .WillOnce(testing::Return(true));
  controller.poll();
}

TEST_F(input_controller, should_check_mouse_button_down) {
  std::ignore = controller.mouse().register_callback(null_callback);
  controller.mouse().monitor_input(jage::input::mouse::buttons::left_click);

  EXPECT_CALL(driver, mouse_is_down(jage::input::mouse::buttons::left_click))
      .WillOnce(testing::Return(true));
  controller.poll();
}

TEST_F(input_controller, should_check_cursor_position) {
  std::ignore = controller.cursor().register_callback(null_callback);
  EXPECT_CALL(driver, cursor_position()).Times(1);
  controller.poll();
}