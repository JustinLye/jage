#include <jage/input/contexts/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/mouse/events/vertical_scroll.hpp>
#include <jage/time/durations.hpp>

#include <jage/test/fakes/containers/event_sink.hpp>

#include <gtest/gtest.h>

#include <variant>

using duration_type = jage::time::durations::nanoseconds;
using event_type = jage::input::event<duration_type>;
using sink_type = jage::test::fakes::containers::event_sink<event_type>;
using context_type = jage::input::contexts::glfw<duration_type, sink_type>;
using scroll_type = jage::input::mouse::events::vertical_scroll;

class glfw_context : public testing::Test {
protected:
  sink_type sink;
  context_type context{sink};
};

TEST_F(glfw_context, should_forward_rvalue_event_to_sink) {
  context.push(event_type{duration_type{42}, scroll_type{1.5}});
  ASSERT_EQ(1, sink.events.size());
}

TEST_F(glfw_context, should_forward_const_lvalue_event_to_sink) {
  const auto event = event_type{duration_type{99}, scroll_type{2.5}};
  context.push(event);
  ASSERT_EQ(1, sink.events.size());
}

TEST_F(glfw_context, should_preserve_timestamp_on_rvalue_push) {
  context.push(event_type{duration_type{42}, scroll_type{1.5}});
  EXPECT_EQ(42, sink.events.front().timestamp.count());
}

TEST_F(glfw_context, should_preserve_payload_on_rvalue_push) {
  context.push(event_type{duration_type{42}, scroll_type{1.5}});
  ASSERT_TRUE(
      std::holds_alternative<scroll_type>(sink.events.front().payload));
  EXPECT_DOUBLE_EQ(
      1.5, std::get<scroll_type>(sink.events.front().payload).offset);
}

TEST_F(glfw_context, should_preserve_timestamp_on_const_lvalue_push) {
  const auto event = event_type{duration_type{99}, scroll_type{2.5}};
  context.push(event);
  EXPECT_EQ(99, sink.events.front().timestamp.count());
}

TEST_F(glfw_context, should_preserve_payload_on_const_lvalue_push) {
  const auto event = event_type{duration_type{99}, scroll_type{2.5}};
  context.push(event);
  ASSERT_TRUE(
      std::holds_alternative<scroll_type>(sink.events.front().payload));
  EXPECT_DOUBLE_EQ(
      2.5, std::get<scroll_type>(sink.events.front().payload).offset);
}

TEST_F(glfw_context, should_forward_multiple_events_in_order) {
  context.push(event_type{duration_type{1}, scroll_type{10.0}});
  context.push(event_type{duration_type{2}, scroll_type{20.0}});
  context.push(event_type{duration_type{3}, scroll_type{30.0}});
  ASSERT_EQ(3, sink.events.size());
  EXPECT_EQ(1, sink.events[0].timestamp.count());
  EXPECT_EQ(2, sink.events[1].timestamp.count());
  EXPECT_EQ(3, sink.events[2].timestamp.count());
}

TEST_F(glfw_context, should_default_cursor_position_to_origin) {
  const auto &position = context.last_known_cursor_position();
  EXPECT_DOUBLE_EQ(0.0, position.first);
  EXPECT_DOUBLE_EQ(0.0, position.second);
}

TEST_F(glfw_context, should_allow_cursor_position_mutation) {
  auto &position = context.last_known_cursor_position();
  position = {1.5, 2.5};
  EXPECT_DOUBLE_EQ(1.5, context.last_known_cursor_position().first);
  EXPECT_DOUBLE_EQ(2.5, context.last_known_cursor_position().second);
}

TEST_F(glfw_context,
       should_reflect_cursor_mutation_through_const_accessor) {
  context.last_known_cursor_position() = {3.14, 2.72};
  const auto &const_context = context;
  EXPECT_DOUBLE_EQ(3.14, const_context.last_known_cursor_position().first);
  EXPECT_DOUBLE_EQ(2.72, const_context.last_known_cursor_position().second);
}
