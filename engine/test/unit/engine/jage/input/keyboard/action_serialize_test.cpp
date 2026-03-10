#include <jage/input/keyboard/action.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <fmt/format.h>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using jage::input::keyboard::action;
using jage::input::keyboard::serialize;
using test_parameter_type = std::pair<action, std::string>;

struct input_keyboard_action_serialize_test
    : public testing::TestWithParam<test_parameter_type> {};

TEST_P(input_keyboard_action_serialize_test,
       should_serialize_action_to_enum_member_name) {
  const auto &[input, expected] = GetParam();
  EXPECT_EQ(serialize(input), expected);
}

static constexpr auto unknown_action_enumerator_0 = static_cast<action>(
    std::numeric_limits<std::underlying_type_t<action>>::max());
static constexpr auto unknown_action_enumerator_1 = static_cast<action>(
    std::numeric_limits<std::underlying_type_t<action>>::max() - 1);

const auto test_parameters = std::vector<test_parameter_type>{
    {action::release, "release"},
    {action::press, "press"},
    {action::repeat, "repeat"},
    {unknown_action_enumerator_0, "unknown enumerator"},
    {unknown_action_enumerator_1, "unknown enumerator"},
};

INSTANTIATE_TEST_SUITE_P(
    input_keyboard_action_serialize, input_keyboard_action_serialize_test,
    testing::ValuesIn(test_parameters), [](const auto &info) -> std::string {
      auto expected = info.param.second;
      std::replace(std::begin(expected), std::end(expected), ' ', '_');
      return fmt::format("input_{}_expected_{}",
                         std::to_underlying(info.param.first), expected);
    });
