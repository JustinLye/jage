#include <jage/input/mouse/button.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using jage::input::mouse::button;
using jage::input::mouse::serialize;
using test_parameter_type = std::pair<button, std::string>;

struct input_mouse_button_serialize_test
    : public testing::TestWithParam<test_parameter_type> {};

TEST_P(input_mouse_button_serialize_test,
       should_serialize_button_to_enum_member_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(serialize(input), expected);
}

static constexpr auto unknown_button_enumerator_0 = static_cast<button>(
    std::numeric_limits<std::underlying_type_t<button>>::max());
static constexpr auto unknown_button_enumerator_1 = static_cast<button>(
    std::numeric_limits<std::underlying_type_t<button>>::max() - 1);

const auto test_parameters = std::vector<test_parameter_type>{
    {button::left, "left"},
    {button::right, "right"},
    {button::middle, "middle"},
    {button::back, "back"},
    {button::forward, "forward"},
    {button::gesture, "gesture"},
    {button::action, "action"},
    {button::unidentified, "unidentified"},
    {unknown_button_enumerator_0, "unknown enumerator"},
    {unknown_button_enumerator_1, "unknown enumerator"},
};

INSTANTIATE_TEST_SUITE_P(
    input_mouse_button_serialize, input_mouse_button_serialize_test,
    testing::ValuesIn(test_parameters), [](const auto &info) -> std::string {
      auto expected = info.param.second;
      std::replace(std::begin(expected), std::end(expected), ' ', '_');
      return fmt::format("input_{}_expected_{}",
                         std::to_underlying(info.param.first), expected);
    });
