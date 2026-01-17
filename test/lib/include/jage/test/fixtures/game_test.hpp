

#include <jage/game.hpp>
#include <jage/input/controller.hpp>
#include <jage/window.hpp>

#include <jage/test/mocks/game/driver.hpp>
#include <jage/test/mocks/input/controller/driver.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace jage::test::fixtures {
struct game_test : public testing::Test {
  testing::NiceMock<mocks::game::driver> window_driver{};
  jage::window<mocks::game::driver> window{window_driver};
  testing::NiceMock<mocks::input::controller::driver> input_driver{};
  jage::input::controller<mocks::input::controller::driver> controller{
      input_driver};

  jage::game<jage::window<mocks::game::driver>,
             jage::input::controller<mocks::input::controller::driver>>
      game{window, controller};

  static constexpr auto null_callback = [](const auto) -> void {};

  void SetUp() override {
    using testing::_;
    ON_CALL(window_driver, should_close()).WillByDefault(testing::Return(true));
    ON_CALL(window_driver, poll()).WillByDefault(testing::Return());
    ON_CALL(window_driver, render()).WillByDefault(testing::Return());
    ON_CALL(input_driver, keyboard_is_down(_))
        .WillByDefault(testing::Return(false));
    ON_CALL(input_driver, mouse_is_down(_))
        .WillByDefault(testing::Return(false));
    ON_CALL(input_driver, cursor_position())
        .WillByDefault(testing::Return(jage::input::cursor::state{}));
  }
};
} // namespace jage::test::fixtures
