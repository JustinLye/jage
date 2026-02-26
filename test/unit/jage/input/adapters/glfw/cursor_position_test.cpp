#include <jage/input/adapters/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/mouse/events/cursor/motion.hpp>
#include <jage/input/mouse/events/cursor/position.hpp>
#include <jage/time/durations.hpp>

#include <jage/test/fakes/input/context/glfw.hpp>
#include <jage/test/fakes/input/platforms/glfw.hpp>

#include <gtest/gtest.h>

#include <variant>

using duration_type = jage::time::durations::nanoseconds;
using context_type =
    jage::test::fakes::input::context::glfw<jage::time::durations::nanoseconds,
                                            jage::input::event<duration_type>>;
using platform_type = jage::test::fakes::input::platforms::glfw<context_type>;
using adapter_type = jage::input::adapters::glfw<platform_type>;
using cursor_position_event_type = jage::input::mouse::events::cursor::position<
    jage::time::durations::nanoseconds>;
using cursor_motion_event_type = jage::input::mouse::events::cursor::motion<
    jage::time::durations::nanoseconds>;

class glfw_adapter : public testing::Test {
protected:
  context_type context;
  platform_type platform;

  auto SetUp() -> void override {
    platform.set_window_user_pointer(nullptr, static_cast<void *>(&context));
  }
  auto TearDown() -> void override { platform.reset(); }
};

TEST_F(glfw_adapter, should_send_cursor_position_event) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.trigger_cursor_position_callback(1.52, 4.20);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<cursor_position_event_type>(
      context.buffer.front()));
  const auto event =
      std::get<cursor_position_event_type>(context.buffer.front());
  EXPECT_EQ(42, event.timestamp.count());
  EXPECT_DOUBLE_EQ(1.52, event.x);
  EXPECT_DOUBLE_EQ(4.20, event.y);
}

TEST_F(glfw_adapter,
       should_not_send_cursor_position_event_if_cursor_has_not_updated) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.trigger_cursor_position_callback(1, 1);
  context.buffer.clear();
  platform.trigger_cursor_position_callback(
      1 + std::numeric_limits<double>::epsilon(), 1);
  ASSERT_TRUE(std::empty(context.buffer));
}

TEST_F(glfw_adapter, should_send_event_after_accumulating_small_updates) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.trigger_cursor_position_callback(1, 1);
  context.buffer.clear();
  platform.trigger_cursor_position_callback(
      1 + std::numeric_limits<double>::epsilon(), 1);
  ASSERT_TRUE(std::empty(context.buffer));
  platform.trigger_cursor_position_callback(
      1 + std::numeric_limits<double>::epsilon() +
          std::numeric_limits<double>::epsilon(),
      1);
  EXPECT_FALSE(std::empty(context.buffer));
}

TEST_F(
    glfw_adapter,
    should_not_send_cursor_motion_event_after_disabling_cursor_without_motion) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(0, 0);
  EXPECT_TRUE(std::empty(context.buffer));
}

TEST_F(glfw_adapter, should_send_motion_after_motion_when_cursor_is_disabled) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{99});
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(1.5, 4.242);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<cursor_motion_event_type>(context.buffer.front()));
  const auto event = std::get<cursor_motion_event_type>(context.buffer.front());
  EXPECT_EQ(99, event.timestamp.count());
  EXPECT_DOUBLE_EQ(1.50, event.delta_x);
  EXPECT_DOUBLE_EQ(4.242, event.delta_y);
}

TEST_F(glfw_adapter, should_update_motion_delta_values) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{99});
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(1.5, 4.242);
  context.buffer.clear();
  platform.trigger_cursor_position_callback(2.25, 5.0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<cursor_motion_event_type>(context.buffer.front()));
  const auto event = std::get<cursor_motion_event_type>(context.buffer.front());
  EXPECT_EQ(99, event.timestamp.count());
  EXPECT_DOUBLE_EQ(0.75, event.delta_x);
  EXPECT_DOUBLE_EQ(0.758, event.delta_y);
}

TEST_F(glfw_adapter,
       should_not_update_motion_delta_values_until_position_changes) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{99});
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(1.5, 4.242);
  platform.trigger_cursor_position_callback(2.25, 5.0);
  context.buffer.clear();
  platform.trigger_cursor_position_callback(2.25, 5.0);
  platform.trigger_cursor_position_callback(2.25, 5.0);
  ASSERT_TRUE(std::empty(context.buffer));
  platform.trigger_cursor_position_callback(2.256, 5.0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<cursor_motion_event_type>(context.buffer.front()));
  const auto event = std::get<cursor_motion_event_type>(context.buffer.front());
  EXPECT_NEAR(0.006, event.delta_x, 1e-12);
  EXPECT_DOUBLE_EQ(0.0, event.delta_y);
}

TEST_F(glfw_adapter, should_not_send_event_small_change) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(1, 1);
  context.buffer.clear();
  platform.trigger_cursor_position_callback(
      1 + std::numeric_limits<double>::epsilon(), 1);
  ASSERT_TRUE(std::empty(context.buffer));
  platform.trigger_cursor_position_callback(
      1 + std::numeric_limits<double>::epsilon() +
          std::numeric_limits<double>::epsilon(),
      1);
  EXPECT_FALSE(std::empty(context.buffer));
}

TEST_F(glfw_adapter, should_transition_correctly) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{42});
  platform.trigger_cursor_position_callback(1, 1);
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(1.5, 2.5);

  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<cursor_position_event_type>(
      context.buffer.front()));
  {
    const auto event =
        std::get<cursor_position_event_type>(context.buffer.front());
    EXPECT_DOUBLE_EQ(1, event.x);
    EXPECT_DOUBLE_EQ(1, event.y);
  }
  context.buffer.pop_front();
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<cursor_motion_event_type>(context.buffer.front()));
  {
    const auto event =
        std::get<cursor_motion_event_type>(context.buffer.front());
    EXPECT_NEAR(0.5, event.delta_x, 1e-12);
    EXPECT_NEAR(1.5, event.delta_y, 1e-12);
  }
  context.buffer.pop_front();
  ASSERT_TRUE(std::empty(context.buffer));
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  platform.trigger_cursor_position_callback(1.5, 2.5);
  ASSERT_TRUE(std::empty(context.buffer));
  platform.trigger_cursor_position_callback(99.42, 42.99);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<cursor_position_event_type>(
      context.buffer.front()));
  {
    const auto event =
        std::get<cursor_position_event_type>(context.buffer.front());
    EXPECT_DOUBLE_EQ(99.42, event.x);
    EXPECT_DOUBLE_EQ(42.99, event.y);
  }
  context.buffer.pop_front();
  ASSERT_TRUE(std::empty(context.buffer));
  platform.set_input_mode(nullptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  platform.trigger_cursor_position_callback(99.42, 42.99);
  ASSERT_TRUE(std::empty(context.buffer));
  platform.trigger_cursor_position_callback(99.43, 43.01);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<cursor_motion_event_type>(context.buffer.front()));
  {
    const auto event =
        std::get<cursor_motion_event_type>(context.buffer.front());
    EXPECT_NEAR(0.01, event.delta_x, 1e-12);
    EXPECT_NEAR(0.02, event.delta_y, 1e-12);
  }
}
