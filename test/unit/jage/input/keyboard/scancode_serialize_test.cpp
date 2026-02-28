#include <jage/input/keyboard/scancode.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using jage::input::keyboard::scancode;
using jage::input::keyboard::serialize;
using test_parameter_type = std::pair<scancode, std::string>;

struct input_keyboard_scancode_serialize_test
    : public testing::TestWithParam<test_parameter_type> {};

TEST_P(input_keyboard_scancode_serialize_test,
       should_serialize_scancode_to_enum_member_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(serialize(input), expected);
}

static constexpr auto unknown_scancode_enumerator_0 = static_cast<scancode>(
    std::numeric_limits<std::underlying_type_t<scancode>>::max());
static constexpr auto unknown_scancode_enumerator_1 = static_cast<scancode>(
    std::numeric_limits<std::underlying_type_t<scancode>>::max() - 1);

const auto test_parameters = std::vector<test_parameter_type>{
    {scancode::unidentified, "unidentified"},
    {scancode::a, "a"},
    {scancode::b, "b"},
    {scancode::c, "c"},
    {scancode::d, "d"},
    {scancode::e, "e"},
    {scancode::f, "f"},
    {scancode::g, "g"},
    {scancode::h, "h"},
    {scancode::i, "i"},
    {scancode::j, "j"},
    {scancode::k, "k"},
    {scancode::l, "l"},
    {scancode::m, "m"},
    {scancode::n, "n"},
    {scancode::o, "o"},
    {scancode::p, "p"},
    {scancode::q, "q"},
    {scancode::r, "r"},
    {scancode::s, "s"},
    {scancode::t, "t"},
    {scancode::u, "u"},
    {scancode::v, "v"},
    {scancode::w, "w"},
    {scancode::x, "x"},
    {scancode::y, "y"},
    {scancode::z, "z"},
    {scancode::_1, "1"},
    {scancode::_2, "2"},
    {scancode::_3, "3"},
    {scancode::_4, "4"},
    {scancode::_5, "5"},
    {scancode::_6, "6"},
    {scancode::_7, "7"},
    {scancode::_8, "8"},
    {scancode::_9, "9"},
    {scancode::_0, "0"},
    {scancode::enter, "enter"},
    {scancode::escape, "escape"},
    {scancode::backspace, "backspace"},
    {scancode::tab, "tab"},
    {scancode::spacebar, "spacebar"},
    {scancode::minus, "minus"},
    {scancode::equal, "equal"},
    {scancode::left_bracket, "left_bracket"},
    {scancode::right_bracket, "right_bracket"},
    {scancode::backslash, "backslash"},
    {scancode::semicolon, "semicolon"},
    {scancode::apostrophe, "apostrophe"},
    {scancode::grave_accent, "grave_accent"},
    {scancode::comma, "comma"},
    {scancode::period, "period"},
    {scancode::slash, "slash"},
    {scancode::caps_lock, "caps_lock"},
    {scancode::F1, "F1"},
    {scancode::F2, "F2"},
    {scancode::F3, "F3"},
    {scancode::F4, "F4"},
    {scancode::F5, "F5"},
    {scancode::F6, "F6"},
    {scancode::F7, "F7"},
    {scancode::F8, "F8"},
    {scancode::F9, "F9"},
    {scancode::F10, "F10"},
    {scancode::F11, "F11"},
    {scancode::F12, "F12"},
    {scancode::print_screen, "print_screen"},
    {scancode::scroll_lock, "scroll_lock"},
    {scancode::pause, "pause"},
    {scancode::insert, "insert"},
    {scancode::home, "home"},
    {scancode::page_up, "page_up"},
    {scancode::delete_key, "delete_key"},
    {scancode::end, "end"},
    {scancode::page_down, "page_down"},
    {scancode::arrow_right, "arrow_right"},
    {scancode::arrow_left, "arrow_left"},
    {scancode::arrow_down, "arrow_down"},
    {scancode::arrow_up, "arrow_up"},
    {scancode::num_lock, "num_lock"},
    {scancode::kp_divide, "kp_divide"},
    {scancode::kp_multiply, "kp_multiply"},
    {scancode::kp_subtract, "kp_subtract"},
    {scancode::kp_add, "kp_add"},
    {scancode::kp_enter, "kp_enter"},
    {scancode::kp_1, "kp_1"},
    {scancode::kp_2, "kp_2"},
    {scancode::kp_3, "kp_3"},
    {scancode::kp_4, "kp_4"},
    {scancode::kp_5, "kp_5"},
    {scancode::kp_6, "kp_6"},
    {scancode::kp_7, "kp_7"},
    {scancode::kp_8, "kp_8"},
    {scancode::kp_9, "kp_9"},
    {scancode::kp_0, "kp_0"},
    {scancode::kp_decimal, "kp_decimal"},
    {scancode::world_1, "world_1"},
    {scancode::world_2, "world_2"},
    {scancode::kp_equal, "kp_equal"},
    {scancode::menu, "menu"},
    {scancode::F13, "F13"},
    {scancode::F14, "F14"},
    {scancode::F15, "F15"},
    {scancode::F16, "F16"},
    {scancode::F17, "F17"},
    {scancode::F18, "F18"},
    {scancode::F19, "F19"},
    {scancode::F20, "F20"},
    {scancode::F21, "F21"},
    {scancode::F22, "F22"},
    {scancode::F23, "F23"},
    {scancode::F24, "F24"},
    {scancode::execute, "execute"},
    {scancode::left_control, "left_control"},
    {scancode::left_shift, "left_shift"},
    {scancode::left_alt, "left_alt"},
    {scancode::left_super, "left_super"},
    {scancode::right_control, "right_control"},
    {scancode::right_shift, "right_shift"},
    {scancode::right_alt, "right_alt"},
    {scancode::right_super, "right_super"},
    {unknown_scancode_enumerator_0, "unknown enumerator"},
    {unknown_scancode_enumerator_1, "unknown enumerator"},
};

INSTANTIATE_TEST_SUITE_P(
    input_keyboard_scancode_serialize, input_keyboard_scancode_serialize_test,
    testing::ValuesIn(test_parameters), [](const auto &info) -> std::string {
      auto expected = info.param.second;
      std::replace(std::begin(expected), std::end(expected), ' ', '_');
      return fmt::format("input_{}_expected_{}",
                         std::to_underlying(info.param.first), expected);
    });
