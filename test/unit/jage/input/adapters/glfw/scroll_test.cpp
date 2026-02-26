#include <jage/input/adapters/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/mouse/events/horizontal_scroll.hpp>
#include <jage/input/mouse/events/vertical_scroll.hpp>
#include <jage/time/durations.hpp>

#include <jage/test/fakes/input/context/glfw.hpp>
#include <jage/test/fakes/input/platforms/glfw.hpp>

#include <gtest/gtest.h>

#include <variant>

using duration_type = jage::time::durations::nanoseconds;
using context_type =
    jage::test::fakes::input::context::glfw<duration_type,
                                            jage::input::event<duration_type>>;
using platform_type = jage::test::fakes::input::platforms::glfw<context_type>;
using horizontal_scroll_event =
    jage::input::mouse::events::horizontal_scroll<duration_type>;
using vertical_scroll_event =
    jage::input::mouse::events::vertical_scroll<duration_type>;
using adapter_type = jage::input::adapters::glfw<platform_type>;

class glfw_adapter : public testing::Test {
protected:
  context_type context;
  platform_type platform;

  auto SetUp() -> void override {
    platform.set_window_user_pointer(nullptr, static_cast<void *>(&context));
  }
};

TEST_F(glfw_adapter, should_push_vertical_scroll_event_on_y_offset_change) {
  std::ignore = adapter_type{nullptr, platform, duration_type{42}};
  platform.trigger_scroll_callback(0.0, 1.75);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<vertical_scroll_event>(context.buffer.front()));
  const auto event = std::get<vertical_scroll_event>(context.buffer.front());
  EXPECT_EQ(42, event.timestamp.count());
  EXPECT_EQ(1.75, event.offset);
}

TEST_F(glfw_adapter, should_push_horizontal_scroll_event_on_x_offset_change) {
  std::ignore = adapter_type{nullptr, platform, duration_type{100}};
  platform.trigger_scroll_callback(2.5, 0.0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(
      std::holds_alternative<horizontal_scroll_event>(context.buffer.front()));
  const auto event = std::get<horizontal_scroll_event>(context.buffer.front());
  EXPECT_EQ(100, event.timestamp.count());
  EXPECT_EQ(2.5, event.offset);
}
