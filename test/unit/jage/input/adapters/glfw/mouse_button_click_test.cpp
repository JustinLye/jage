#include <jage/input/adapters/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/keyboard/event.hpp>
#include <jage/input/mouse/action.hpp>
#include <jage/input/mouse/button.hpp>
#include <jage/input/mouse/events/click.hpp>
#include <jage/time/durations.hpp>

#include <GLFW/glfw3.h>

#include <jage/test/fakes/input/context/glfw.hpp>
#include <jage/test/fakes/input/platforms/glfw.hpp>

#include <gtest/gtest.h>

#include <array>
#include <optional>
#include <utility>

using duration_type = jage::time::durations::nanoseconds;
using mouse_click_event = jage::input::mouse::events::click<duration_type>;
using key_press_event = jage::input::keyboard::event<duration_type>;
using context_type =
    jage::test::fakes::input::context::glfw<duration_type,
                                            jage::input::event<duration_type>>;
using platform_type = jage::test::fakes::input::platforms::glfw<context_type>;
using adapter_type = jage::input::adapters::glfw<platform_type>;

namespace mouse = jage::input::mouse;
using jage::input::modifier;

static_assert(std::to_underlying(mouse::button::last) ==
              GLFW_MOUSE_BUTTON_LAST);

class glfw_adapter {
protected:
  context_type context;
  platform_type platform;

  auto setup() -> void {
    platform.set_window_user_pointer(
        nullptr, static_cast<platform_type::user_pointer_type>(&context));
  }

  auto teardown() -> void { platform.reset(); }
};

template <class...> class glfw_adapter_test;

struct no_test_param;

template <>
class glfw_adapter_test<no_test_param> : protected glfw_adapter,
                                         public testing::Test {
protected:
  auto SetUp() -> void override { glfw_adapter::setup(); }
  auto TearDown() -> void override { glfw_adapter::teardown(); }
};

template <class TParam>
class glfw_adapter_test<TParam> : protected glfw_adapter,
                                  public testing::TestWithParam<TParam> {
protected:
  auto SetUp() -> void override { glfw_adapter::setup(); }
  auto TearDown() -> void override { glfw_adapter::teardown(); }
};

struct button_param {
  int input_button;
  mouse::button expected_output_button;
};

using glfw_adapter_button_click = glfw_adapter_test<button_param>;

TEST_P(glfw_adapter_button_click, should_map_button_click) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  const auto &[glfw_button, expected_output_button] = GetParam();
  platform.trigger_mouse_button_callback(glfw_button, GLFW_PRESS, 0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<mouse_click_event>(context.buffer.front()));
  const auto event = std::get<mouse_click_event>(context.buffer.front());
  EXPECT_EQ(expected_output_button, event.button);
}

struct modifier_param {
  int glfw_modifier;
  modifier expected_left;
  std::optional<modifier> expected_right;
};

using glfw_adapter_button_click_modifier = glfw_adapter_test<modifier_param>;

TEST_P(glfw_adapter_button_click_modifier, should_map_modifier) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  const auto &[glfw_modifier, expected_left, optional_expected_right] =
      GetParam();
  platform.trigger_mouse_button_callback(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS,
                                         glfw_modifier);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<mouse_click_event>(context.buffer.front()));
  const auto event = std::get<mouse_click_event>(context.buffer.front());
  EXPECT_TRUE(event.modifiers.test(
      static_cast<std::size_t>(std::to_underlying(expected_left))));
  optional_expected_right.and_then([&](const auto expected_right) {
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(expected_right))));
    return std::optional{expected_right};
  });
}

struct action_param {
  int glfw_action;
  mouse::action expected_action;
};

using glfw_adapter_button_click_action = glfw_adapter_test<action_param>;
TEST_P(glfw_adapter_button_click_action, should_map_action) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  const auto &[glfw_action, expected_action] = GetParam();
  platform.trigger_mouse_button_callback(GLFW_MOUSE_BUTTON_LEFT, glfw_action,
                                         0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<mouse_click_event>(context.buffer.front()));
  const auto event = std::get<mouse_click_event>(context.buffer.front());
  EXPECT_EQ(expected_action, event.action);
}

using glfw_adapter_timestamp_test = glfw_adapter_test<no_test_param>;

TEST_F(glfw_adapter_timestamp_test, should_set_timestamp) {
  const auto initial_duration = context_type::duration_type{42e+9};

  adapter_type::initialize(nullptr, platform, initial_duration);
  platform.trigger_mouse_button_callback(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(
        std::holds_alternative<mouse_click_event>(context.buffer.front()));
    const auto event = std::get<mouse_click_event>(context.buffer.front());
    EXPECT_EQ(initial_duration, event.timestamp);
  }
  context.buffer.clear();
  const auto next_duration = context_type::duration_type{99e+9};
  platform.set_seconds_since_init(next_duration);
  platform.trigger_mouse_button_callback(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(
        std::holds_alternative<mouse_click_event>(context.buffer.front()));
    const auto event = std::get<mouse_click_event>(context.buffer.front());
    EXPECT_EQ(next_duration, event.timestamp);
  }
}

INSTANTIATE_TEST_SUITE_P(
    button_mappings, glfw_adapter_button_click,
    testing::ValuesIn(std::array{
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_1,
            .expected_output_button = mouse::button::left,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_LEFT,
            .expected_output_button = mouse::button::left,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_2,
            .expected_output_button = mouse::button::right,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_RIGHT,
            .expected_output_button = mouse::button::right,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_3,
            .expected_output_button = mouse::button::middle,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_MIDDLE,
            .expected_output_button = mouse::button::middle,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_4,
            .expected_output_button = mouse::button::back,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_5,
            .expected_output_button = mouse::button::forward,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_6,
            .expected_output_button = mouse::button::gesture,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_7,
            .expected_output_button = mouse::button::action,
        },
        button_param{
            .input_button = GLFW_MOUSE_BUTTON_8,
            .expected_output_button = mouse::button::unidentified,
        },
    }));

INSTANTIATE_TEST_SUITE_P(modifier_mappings, glfw_adapter_button_click_modifier,
                         testing::ValuesIn(std::array{
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_SHIFT,
                                 .expected_left = modifier::left_shift,
                                 .expected_right = modifier::right_shift,
                             },
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_CONTROL,
                                 .expected_left = modifier::left_control,
                                 .expected_right = modifier::right_control,
                             },
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_ALT,
                                 .expected_left = modifier::left_alt,
                                 .expected_right = modifier::right_alt,
                             },
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_SUPER,
                                 .expected_left = modifier::left_gui,
                                 .expected_right = modifier::right_gui,
                             },
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_CAPS_LOCK,
                                 .expected_left = modifier::caps_lock,
                                 .expected_right = std::nullopt,
                             },
                             modifier_param{
                                 .glfw_modifier = GLFW_MOD_NUM_LOCK,
                                 .expected_left = modifier::num_lock,
                                 .expected_right = std::nullopt,
                             },

                         }));

INSTANTIATE_TEST_SUITE_P(action_mapping, glfw_adapter_button_click_action,
                         ::testing::ValuesIn(std::array{
                             action_param{
                                 .glfw_action = GLFW_PRESS,
                                 .expected_action = mouse::action::press,
                             },
                             action_param{
                                 .glfw_action = GLFW_RELEASE,
                                 .expected_action = mouse::action::release,
                             },
                         }));