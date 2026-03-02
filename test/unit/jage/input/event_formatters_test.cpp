#include <jage/input/event_formatters.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <bitset>
#include <chrono>
#include <cstdint>
#include <fmt/format.h>
#include <string>
#include <utility>
#include <vector>

using namespace jage::input;
using namespace jage::time;

using keyboard_key_param = std::pair<keyboard::key, std::string>;

struct keyboard_key_format_test : testing::TestWithParam<keyboard_key_param> {};

TEST_P(keyboard_key_format_test, should_format_key_as_serialized_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto keyboard_key_format_params = std::vector<keyboard_key_param>{
    {keyboard::key::a, "a"},
    {keyboard::key::spacebar, "spacebar"},
    {keyboard::key::escape, "escape"},
    {keyboard::key::F1, "F1"},
};

INSTANTIATE_TEST_SUITE_P(keyboard_key_format, keyboard_key_format_test,
                         testing::ValuesIn(keyboard_key_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("key_{}", info.param.second);
                         });

using keyboard_action_param = std::pair<keyboard::action, std::string>;

struct keyboard_action_format_test
    : testing::TestWithParam<keyboard_action_param> {};

TEST_P(keyboard_action_format_test, should_format_action_as_serialized_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto keyboard_action_format_params = std::vector<keyboard_action_param>{
    {keyboard::action::press, "press"},
    {keyboard::action::release, "release"},
    {keyboard::action::repeat, "repeat"},
};

INSTANTIATE_TEST_SUITE_P(keyboard_action_format, keyboard_action_format_test,
                         testing::ValuesIn(keyboard_action_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("action_{}", info.param.second);
                         });

using keyboard_scancode_param = std::pair<keyboard::scancode, std::string>;

struct keyboard_scancode_format_test
    : testing::TestWithParam<keyboard_scancode_param> {};

TEST_P(keyboard_scancode_format_test,
       should_format_scancode_as_serialized_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto keyboard_scancode_format_params =
    std::vector<keyboard_scancode_param>{
        {keyboard::scancode::a, "a"},
        {keyboard::scancode::enter, "enter"},
        {keyboard::scancode::F12, "F12"},
    };

INSTANTIATE_TEST_SUITE_P(keyboard_scancode_format,
                         keyboard_scancode_format_test,
                         testing::ValuesIn(keyboard_scancode_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("scancode_{}", info.param.second);
                         });

using mouse_button_param = std::pair<mouse::button, std::string>;

struct mouse_button_format_test : testing::TestWithParam<mouse_button_param> {};

TEST_P(mouse_button_format_test, should_format_button_as_serialized_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto mouse_button_format_params = std::vector<mouse_button_param>{
    {mouse::button::left, "left"},
    {mouse::button::right, "right"},
    {mouse::button::middle, "middle"},
};

INSTANTIATE_TEST_SUITE_P(mouse_button_format, mouse_button_format_test,
                         testing::ValuesIn(mouse_button_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("button_{}", info.param.second);
                         });

using mouse_action_param = std::pair<mouse::action, std::string>;

struct mouse_action_format_test : testing::TestWithParam<mouse_action_param> {};

TEST_P(mouse_action_format_test, should_format_action_as_serialized_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto mouse_action_format_params = std::vector<mouse_action_param>{
    {mouse::action::press, "press"},
    {mouse::action::release, "release"},
};

INSTANTIATE_TEST_SUITE_P(mouse_action_format, mouse_action_format_test,
                         testing::ValuesIn(mouse_action_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("action_{}", info.param.second);
                         });

using mouse_click_param = std::pair<mouse::events::click, std::string>;

struct mouse_click_format_test : testing::TestWithParam<mouse_click_param> {};

TEST_P(mouse_click_format_test, should_format_click_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto mouse_click_format_params = std::vector<mouse_click_param>{
    {mouse::events::click{.button = mouse::button::left,
                          .action = mouse::action::press,
                          .modifiers = std::bitset<modifier_count>{}},
     R"("mouse-click": {
      "button": left,
      "action": press,
      "modifiers": 0000000000
  })"},
    {mouse::events::click{.button = mouse::button::right,
                          .action = mouse::action::release,
                          .modifiers = std::bitset<modifier_count>{0b11}},
     R"("mouse-click": {
      "button": right,
      "action": release,
      "modifiers": 0000000011
  })"},
};

INSTANTIATE_TEST_SUITE_P(mouse_click_format, mouse_click_format_test,
                         testing::ValuesIn(mouse_click_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using key_press_param = std::pair<keyboard::events::key_press, std::string>;

struct key_press_format_test : testing::TestWithParam<key_press_param> {};

TEST_P(key_press_format_test, should_format_key_press_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto key_press_format_params = std::vector<key_press_param>{
    {keyboard::events::key_press{.key = keyboard::key::a,
                                 .scancode = keyboard::scancode::a,
                                 .action = keyboard::action::press,
                                 .modifiers = std::bitset<modifier_count>{}},
     R"("key-press": {
      "key": a,
      "scancode": a,
      "action": press,
      "modifiers": 0000000000
  })"},
    {keyboard::events::key_press{.key = keyboard::key::escape,
                                 .scancode = keyboard::scancode::escape,
                                 .action = keyboard::action::release,
                                 .modifiers =
                                     std::bitset<modifier_count>{0b101}},
     R"("key-press": {
      "key": escape,
      "scancode": escape,
      "action": release,
      "modifiers": 0000000101
  })"},
};

INSTANTIATE_TEST_SUITE_P(key_press_format, key_press_format_test,
                         testing::ValuesIn(key_press_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using cursor_position_param =
    std::pair<mouse::events::cursor::position, std::string>;

struct cursor_position_format_test
    : testing::TestWithParam<cursor_position_param> {};

TEST_P(cursor_position_format_test, should_format_cursor_position_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto cursor_position_format_params = std::vector<cursor_position_param>{
    {mouse::events::cursor::position{.x = 100.5, .y = 200.5},
     R"("cursor-position": {
      "x": 100.5,
      "y": 200.5
  })"},
    {mouse::events::cursor::position{.x = -1.25, .y = 3.75},
     R"("cursor-position": {
      "x": -1.25,
      "y": 3.75
  })"},
};

INSTANTIATE_TEST_SUITE_P(cursor_position_format, cursor_position_format_test,
                         testing::ValuesIn(cursor_position_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using cursor_motion_param =
    std::pair<mouse::events::cursor::motion, std::string>;

struct cursor_motion_format_test : testing::TestWithParam<cursor_motion_param> {
};

TEST_P(cursor_motion_format_test, should_format_cursor_motion_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto cursor_motion_format_params = std::vector<cursor_motion_param>{
    {mouse::events::cursor::motion{.delta_x = 1.5, .delta_y = -2.5},
     R"("cursor-motion": {
      "delta_x": 1.5,
      "delta_y": -2.5
  })"},
};

INSTANTIATE_TEST_SUITE_P(cursor_motion_format, cursor_motion_format_test,
                         testing::ValuesIn(cursor_motion_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using horizontal_scroll_param =
    std::pair<mouse::events::horizontal_scroll, std::string>;

struct horizontal_scroll_format_test
    : testing::TestWithParam<horizontal_scroll_param> {};

TEST_P(horizontal_scroll_format_test, should_format_horizontal_scroll_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto horizontal_scroll_format_params =
    std::vector<horizontal_scroll_param>{
        {mouse::events::horizontal_scroll{.offset = 3.5},
         R"("horizontal-scroll": {
      "offset": 3.5
  })"},
        {mouse::events::horizontal_scroll{.offset = -1.25},
         R"("horizontal-scroll": {
      "offset": -1.25
  })"},
    };

INSTANTIATE_TEST_SUITE_P(horizontal_scroll_format,
                         horizontal_scroll_format_test,
                         testing::ValuesIn(horizontal_scroll_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using vertical_scroll_param =
    std::pair<mouse::events::vertical_scroll, std::string>;

struct vertical_scroll_format_test
    : testing::TestWithParam<vertical_scroll_param> {};

TEST_P(vertical_scroll_format_test, should_format_vertical_scroll_event) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto vertical_scroll_format_params = std::vector<vertical_scroll_param>{
    {mouse::events::vertical_scroll{.offset = 2.75},
     R"("vertical-scroll": {
      "offset": 2.75
  })"},
};

INSTANTIATE_TEST_SUITE_P(vertical_scroll_format, vertical_scroll_format_test,
                         testing::ValuesIn(vertical_scroll_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

struct jage_ns_param {
  durations::nanoseconds input;
  std::uint64_t expected_ns_count;
  std::string expected_date_prefix;
};

struct jage_nanoseconds_format_test : testing::TestWithParam<jage_ns_param> {};

TEST_P(jage_nanoseconds_format_test,
       should_format_timestamp_with_date_and_ns_count) {
  const auto &param = GetParam();
  auto result = fmt::format("{}", param.input);
  EXPECT_THAT(result, testing::HasSubstr(param.expected_date_prefix));
  EXPECT_THAT(result, testing::HasSubstr(fmt::format("{} ns since epoch",
                                                     param.expected_ns_count)));
}

const auto jage_ns_format_params = std::vector<jage_ns_param>{
    {durations::nanoseconds{0.0}, 0, "1970-01-01"},
    {durations::nanoseconds{86400000000000.0}, 86400000000000, "1970-01-02"},
};

INSTANTIATE_TEST_SUITE_P(jage_nanoseconds_format, jage_nanoseconds_format_test,
                         testing::ValuesIn(jage_ns_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("ns_{}",
                                              info.param.expected_ns_count);
                         });

struct chrono_ns_param {
  std::chrono::nanoseconds input;
  std::uint64_t expected_ns_count;
  std::string expected_date_prefix;
};

struct chrono_nanoseconds_format_test
    : testing::TestWithParam<chrono_ns_param> {};

TEST_P(chrono_nanoseconds_format_test,
       should_format_timestamp_with_cst_adjusted_date_and_ns_count) {
  const auto &param = GetParam();
  auto result = fmt::format("{}", param.input);
  EXPECT_THAT(result, testing::HasSubstr(param.expected_date_prefix));
  EXPECT_THAT(result, testing::HasSubstr(fmt::format("{} ns since epoch",
                                                     param.expected_ns_count)));
}

const auto chrono_ns_format_params = std::vector<chrono_ns_param>{
    {std::chrono::nanoseconds{0}, 0, "1969-12-31"},
    {std::chrono::nanoseconds{21600000000000}, 21600000000000, "1970-01-01"},
};

INSTANTIATE_TEST_SUITE_P(chrono_nanoseconds_format,
                         chrono_nanoseconds_format_test,
                         testing::ValuesIn(chrono_ns_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("ns_{}",
                                              info.param.expected_ns_count);
                         });

using event_type = event<durations::nanoseconds>;
using payload_type = event_type::payload_type;
using payload_param = std::pair<payload_type, std::string>;

struct payload_type_format_test : testing::TestWithParam<payload_param> {};

TEST_P(payload_type_format_test,
       should_format_payload_variant_using_active_type_formatter) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(fmt::format("{}", input), expected);
}

const auto payload_format_params = std::vector<payload_param>{
    {payload_type{mouse::events::cursor::position{.x = 1.5, .y = 2.5}},
     R"("cursor-position": {
      "x": 1.5,
      "y": 2.5
  })"},
    {payload_type{mouse::events::vertical_scroll{.offset = 4.5}},
     R"("vertical-scroll": {
      "offset": 4.5
  })"},
    {payload_type{mouse::events::horizontal_scroll{.offset = 4.5}},
     R"("horizontal-scroll": {
      "offset": 4.5
  })"},
    {payload_type{
         mouse::events::click{.button = mouse::button::left,
                              .action = mouse::action::press,
                              .modifiers = std::bitset<modifier_count>{}}},
     R"("mouse-click": {
      "button": left,
      "action": press,
      "modifiers": 0000000000
  })"},
    {payload_type{mouse::events::cursor::motion{.delta_x = 42, .delta_y = 99}},
     R"("cursor-motion": {
      "delta_x": 42,
      "delta_y": 99
  })"},
    {payload_type{keyboard::events::key_press{
         .key = keyboard::key::escape,
         .scancode = keyboard::scancode::escape,
         .action = keyboard::action::release,
         .modifiers = std::bitset<modifier_count>{0b101}}},
     R"("key-press": {
      "key": escape,
      "scancode": escape,
      "action": release,
      "modifiers": 0000000101
  })"},
};

INSTANTIATE_TEST_SUITE_P(payload_type_format, payload_type_format_test,
                         testing::ValuesIn(payload_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

struct event_param {
  event_type input;
  std::vector<std::string> expected_substrings;
};

struct event_format_test : testing::TestWithParam<event_param> {};

TEST_P(event_format_test, should_format_event_with_timestamp_and_payload) {
  const auto &param = GetParam();
  auto result = fmt::format("{}", param.input);
  for (const auto &substring : param.expected_substrings) {
    EXPECT_THAT(result, testing::HasSubstr(substring));
  }
}

const auto event_format_params = std::vector<event_param>{
    {event_type{.timestamp = durations::nanoseconds{1000000000.0},
                .payload =
                    mouse::events::cursor::position{.x = 5.5, .y = 10.5}},
     {R"("input-event": {)", R"("timestamp":)", R"("cursor-position":)",
      "1000000000 ns since epoch", R"("x": 5.5)", R"("y": 10.5)"}},
    {event_type{.timestamp = durations::nanoseconds{2000000000.0},
                .payload = mouse::events::horizontal_scroll{.offset = 1.5}},
     {R"("input-event": {)", "2000000000 ns since epoch",
      R"("horizontal-scroll":)", R"("offset": 1.5)"}},
};

INSTANTIATE_TEST_SUITE_P(event_format, event_format_test,
                         testing::ValuesIn(event_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });

using snapshot_type = events::snapshot<durations::nanoseconds>;

struct snapshot_param {
  snapshot_type input;
  std::vector<std::string> expected_substrings;
};

struct snapshot_format_test : testing::TestWithParam<snapshot_param> {};

TEST_P(snapshot_format_test, should_format_snapshot_with_frame_timing_fields) {
  const auto &param = GetParam();
  auto result = fmt::format("{}", param.input);
  for (const auto &substring : param.expected_substrings) {
    EXPECT_THAT(result, testing::HasSubstr(substring));
  }
}

const auto snapshot_format_params = std::vector<snapshot_param>{
    {snapshot_type{.real_time = durations::nanoseconds{1000000000.0},
                   .tick_duration = durations::nanoseconds{16666666.0},
                   .time_scale = 0.5,
                   .elapsed_time = durations::nanoseconds{500000000.0},
                   .elapsed_frames = 30,
                   .frame = 100,
                   .accumulated_time = durations::nanoseconds{250000000.0}},
     {R"("frame": {)", R"("snapshot": {)", "1000000000 ns since epoch",
      "16666666 ns since epoch", R"("time_scale": 0.5,)",
      "500000000 ns since epoch", R"("elapsed_frames": 30,)",
      R"("frame": 100,)", "250000000 ns since epoch"}},
};

INSTANTIATE_TEST_SUITE_P(snapshot_format, snapshot_format_test,
                         testing::ValuesIn(snapshot_format_params),
                         [](const auto &info) -> std::string {
                           return fmt::format("case_{}", info.index);
                         });
