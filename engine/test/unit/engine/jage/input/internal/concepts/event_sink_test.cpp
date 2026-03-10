#include <jage/input/internal/concepts/event_sink.hpp>

#include <gtest/gtest.h>

struct event {};

using jage::input::internal::concepts::event_sink;

struct valid_sink {
  auto push(event &&) -> void;
  auto push(const event &) -> void;
};

TEST(event_sink_concept, Accept_type_with_push_for_rvalue_and_const_lvalue) {
  EXPECT_TRUE((event_sink<valid_sink, event>));
}

struct missing_push {};

TEST(event_sink_concept, Reject_type_without_push_method) {
  EXPECT_FALSE((event_sink<missing_push, event>));
}

struct wrong_parameter {
  auto push(int) -> void;
};

TEST(event_sink_concept, Reject_type_with_push_that_takes_wrong_parameter) {
  EXPECT_FALSE((event_sink<wrong_parameter, event>));
}

struct non_void_return {
  auto push(event &&) -> int;
  auto push(const event &) -> int;
};

TEST(event_sink_concept, Reject_type_with_push_that_returns_non_void) {
  EXPECT_FALSE((event_sink<non_void_return, event>));
}

struct no_lvalue {
  auto push(event &&) -> void;
};

TEST(event_sink_concept, Reject_type_with_no_lvalue_overload) {
  EXPECT_FALSE((event_sink<no_lvalue, event>));
}
