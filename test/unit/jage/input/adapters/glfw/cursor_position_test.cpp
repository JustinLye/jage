#include <jage/input/adapters/glfw.hpp>
#include <jage/input/event.hpp>
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
  std::ignore = adapter_type{nullptr, platform,
                             platform_type::context_type::duration_type{42}};
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