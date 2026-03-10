#include <jage/input/keyboard/key.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using jage::input::keyboard::key;
using jage::input::keyboard::serialize;
using test_parameter_type = std::pair<key, std::string>;

struct input_keyboard_key_serialize_test
    : public testing::TestWithParam<test_parameter_type> {};

TEST_P(input_keyboard_key_serialize_test,
       should_serialize_key_to_enum_member_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(serialize(input), expected);
}

static constexpr auto unknown_key_enumerator_0 = static_cast<key>(
    std::numeric_limits<std::underlying_type_t<key>>::max());
static constexpr auto unknown_key_enumerator_1 = static_cast<key>(
    std::numeric_limits<std::underlying_type_t<key>>::max() - 1);

const auto test_parameters = std::vector<test_parameter_type>{
    {key::unidentified, "unidentified"},
    {key::spacebar, "spacebar"},
    {key::a, "a"},
    {key::b, "b"},
    {key::c, "c"},
    {key::d, "d"},
    {key::e, "e"},
    {key::f, "f"},
    {key::g, "g"},
    {key::h, "h"},
    {key::i, "i"},
    {key::j, "j"},
    {key::k, "k"},
    {key::l, "l"},
    {key::m, "m"},
    {key::n, "n"},
    {key::o, "o"},
    {key::p, "p"},
    {key::q, "q"},
    {key::r, "r"},
    {key::s, "s"},
    {key::t, "t"},
    {key::u, "u"},
    {key::v, "v"},
    {key::w, "w"},
    {key::x, "x"},
    {key::y, "y"},
    {key::z, "z"},
    {key::F1, "F1"},
    {key::F2, "F2"},
    {key::F3, "F3"},
    {key::F4, "F4"},
    {key::F5, "F5"},
    {key::F6, "F6"},
    {key::F7, "F7"},
    {key::F8, "F8"},
    {key::F9, "F9"},
    {key::F10, "F10"},
    {key::F11, "F11"},
    {key::F12, "F12"},
    {key::F13, "F13"},
    {key::F14, "F14"},
    {key::F15, "F15"},
    {key::F16, "F16"},
    {key::F17, "F17"},
    {key::F18, "F18"},
    {key::F19, "F19"},
    {key::F20, "F20"},
    {key::F21, "F21"},
    {key::F22, "F22"},
    {key::F23, "F23"},
    {key::F24, "F24"},
    {key::execute, "execute"},
    {key::_0, "0"},
    {key::_1, "1"},
    {key::_2, "2"},
    {key::_3, "3"},
    {key::_4, "4"},
    {key::_5, "5"},
    {key::_6, "6"},
    {key::_7, "7"},
    {key::_8, "8"},
    {key::_9, "9"},
    {key::apostrophe, "apostrophe"},
    {key::comma, "comma"},
    {key::minus, "minus"},
    {key::period, "period"},
    {key::slash, "slash"},
    {key::semicolon, "semicolon"},
    {key::equal, "equal"},
    {key::left_bracket, "left_bracket"},
    {key::backslash, "backslash"},
    {key::right_bracket, "right_bracket"},
    {key::grave_accent, "grave_accent"},
    {key::arrow_up, "arrow_up"},
    {key::arrow_down, "arrow_down"},
    {key::arrow_left, "arrow_left"},
    {key::arrow_right, "arrow_right"},
    {key::insert, "insert"},
    {key::delete_key, "delete_key"},
    {key::home, "home"},
    {key::end, "end"},
    {key::page_up, "page_up"},
    {key::page_down, "page_down"},
    {key::enter, "enter"},
    {key::tab, "tab"},
    {key::backspace, "backspace"},
    {key::caps_lock, "caps_lock"},
    {key::left_shift, "left_shift"},
    {key::right_shift, "right_shift"},
    {key::left_control, "left_control"},
    {key::right_control, "right_control"},
    {key::left_alt, "left_alt"},
    {key::right_alt, "right_alt"},
    {key::left_super, "left_super"},
    {key::right_super, "right_super"},
    {key::menu, "menu"},
    {key::print_screen, "print_screen"},
    {key::scroll_lock, "scroll_lock"},
    {key::num_lock, "num_lock"},
    {key::pause, "pause"},
    {key::kp_0, "kp_0"},
    {key::kp_1, "kp_1"},
    {key::kp_2, "kp_2"},
    {key::kp_3, "kp_3"},
    {key::kp_4, "kp_4"},
    {key::kp_5, "kp_5"},
    {key::kp_6, "kp_6"},
    {key::kp_7, "kp_7"},
    {key::kp_8, "kp_8"},
    {key::kp_9, "kp_9"},
    {key::kp_decimal, "kp_decimal"},
    {key::kp_divide, "kp_divide"},
    {key::kp_multiply, "kp_multiply"},
    {key::kp_subtract, "kp_subtract"},
    {key::kp_add, "kp_add"},
    {key::kp_enter, "kp_enter"},
    {key::kp_equal, "kp_equal"},
    {key::world_1, "world_1"},
    {key::world_2, "world_2"},
    {key::escape, "escape"},
    {unknown_key_enumerator_0, "unknown enumerator"},
    {unknown_key_enumerator_1, "unknown enumerator"},
};

INSTANTIATE_TEST_SUITE_P(
    input_keyboard_key_serialize, input_keyboard_key_serialize_test,
    testing::ValuesIn(test_parameters), [](const auto &info) -> std::string {
      auto expected = info.param.second;
      std::replace(std::begin(expected), std::end(expected), ' ', '_');
      return fmt::format("input_{}_expected_{}",
                         std::to_underlying(info.param.first), expected);
    });
