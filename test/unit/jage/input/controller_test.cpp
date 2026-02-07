
#include <jage/input/controller.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <jage/test/fixtures/input/input_controller.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using jage::test::fixtures::input::input_controller;

namespace keyboard = jage::input::keyboard;
namespace mouse = jage::input::mouse;

TEST_F(input_controller, Check_keyboard_button_down) {
  std::ignore = controller.keyboard().register_callback(null_callback);
  controller.keyboard().monitor_input(keyboard::key::a);

  EXPECT_CALL(driver, keyboard_is_down(keyboard::key::a))
      .WillOnce(testing::Return(true));
  controller.poll();
}

TEST_F(input_controller, Check_mouse_button_down) {
  std::ignore = controller.mouse().register_callback(null_callback);
  controller.mouse().monitor_input(mouse::buttons::left_click);

  EXPECT_CALL(driver, mouse_is_down(mouse::buttons::left_click))
      .WillOnce(testing::Return(true));
  controller.poll();
}

TEST_F(input_controller, Check_cursor_position) {
  std::ignore = controller.cursor().register_callback(null_callback);
  EXPECT_CALL(driver, cursor_position()).Times(1);
  controller.poll();
}
