#include <jage/engine/input/adapters/glfw.hpp>
#include <jage/engine/input/event.hpp>
#include <jage/engine/input/mouse/events/horizontal_scroll.hpp>
#include <jage/engine/input/mouse/events/vertical_scroll.hpp>
#include <jage/engine/test/fakes/input/context/glfw.hpp>
#include <jage/engine/test/fakes/input/platforms/glfw.hpp>
#include <jage/engine/time/durations.hpp>

#include <gtest/gtest.h>

#include <variant>

using duration_type = jage::engine::time::durations::nanoseconds;
using context_type = jage::engine::test::fakes::input::context::glfw<
    duration_type, jage::engine::input::event<duration_type>>;
using platform_type =
    jage::engine::test::fakes::input::platforms::glfw<context_type>;
using horizontal_scroll_event =
    jage::engine::input::mouse::events::horizontal_scroll;
using vertical_scroll_event =
    jage::engine::input::mouse::events::vertical_scroll;
using adapter_type = jage::engine::input::adapters::glfw<platform_type>;

class glfw_adapter : public testing::Test {
protected:
  context_type context;
  platform_type platform;

  auto SetUp() -> void override {
    platform.set_window_user_pointer(nullptr, static_cast<void *>(&context));
  }
};

TEST_F(glfw_adapter, should_push_vertical_scroll_event_on_y_offset_change) {
  adapter_type::initialize(nullptr, platform, duration_type{42});
  platform.trigger_scroll_callback(0.0, 1.75);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<vertical_scroll_event>(
      context.buffer.front().payload));
  EXPECT_EQ(42, context.buffer.front().timestamp.count());
  const auto event =
      std::get<vertical_scroll_event>(context.buffer.front().payload);
  EXPECT_EQ(1.75, event.offset);
}

TEST_F(glfw_adapter, should_push_horizontal_scroll_event_on_x_offset_change) {
  adapter_type::initialize(nullptr, platform, duration_type{100});
  platform.trigger_scroll_callback(2.5, 0.0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<horizontal_scroll_event>(
      context.buffer.front().payload));
  EXPECT_EQ(100, context.buffer.front().timestamp.count());
  const auto event =
      std::get<horizontal_scroll_event>(context.buffer.front().payload);
  EXPECT_EQ(2.5, event.offset);
}
