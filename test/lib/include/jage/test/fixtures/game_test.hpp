

#include <jage/game.hpp>
#include <jage/input/controller.hpp>
#include <jage/window.hpp>

#include <jage/test/mocks/game/driver.hpp>
#include <jage/test/mocks/input/controller/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures {
struct game_test : public testing::Test {
  mocks::game::driver window_driver{};
  jage::window<mocks::game::driver> window{window_driver};
  mocks::input::controller::driver input_driver{};
  jage::input::controller<mocks::input::controller::driver> controller{
      input_driver};

  jage::game<jage::window<mocks::game::driver>,
             jage::input::controller<mocks::input::controller::driver>>
      game{window, controller};

  static constexpr auto null_callback = [](const auto) -> void {};
};
} // namespace jage::test::fixtures
