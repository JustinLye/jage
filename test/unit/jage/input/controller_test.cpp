
#include <jage/input/controller.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <jage/test/fixtures/input/input_controller.hpp>

#include <GUnit.h>
#include <gmock/gmock.h>

using jage::test::fixtures::input::input_controller;

GTEST(input_controller) {
  namespace keyboard = jage::input::keyboard;
  namespace mouse = jage::input::mouse;

  SHOULD("Check keyboard button down") {
    std::ignore = controller.keyboard().register_callback(null_callback);
    controller.keyboard().monitor_input(keyboard::keys::a);

    EXPECT_CALL(driver, keyboard_is_down(keyboard::keys::a))
        .WillOnce(testing::Return(true));
    controller.poll();
  }

  SHOULD("Check mouse button down") {
    std::ignore = controller.mouse().register_callback(null_callback);
    controller.mouse().monitor_input(mouse::buttons::left_click);

    EXPECT_CALL(driver, mouse_is_down(mouse::buttons::left_click))
        .WillOnce(testing::Return(true));
    controller.poll();
  }

  SHOULD("Check cursor position") {
    std::ignore = controller.cursor().register_callback(null_callback);
    EXPECT_CALL(driver, cursor_position()).Times(1);
    controller.poll();
  }
}
