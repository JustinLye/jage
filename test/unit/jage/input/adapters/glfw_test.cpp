#include <jage/input/adapters/glfw.hpp>
#include <jage/input/keyboard/event.hpp>
#include <jage/time/durations.hpp>

#include <GLFW/glfw3.h>

#include <jage/test/fakes/input/context/glfw.hpp>
#include <jage/test/fakes/input/platforms/glfw.hpp>

#include <gtest/gtest.h>

#include <variant>

namespace context = jage::test::fakes::input::context;
namespace platforms = jage::test::fakes::input::platforms;
namespace keyboard = jage::input::keyboard;
namespace durations = jage::time::durations;
namespace adapters = jage::input::adapters;

using keyboard_event = keyboard::event<durations::nanoseconds>;
using context_type = context::glfw<durations::nanoseconds,
                                   keyboard::event<durations::nanoseconds>>;
using platform_type = platforms::glfw<context_type>;
using adapter_type = adapters::glfw<platform_type>;

class glfw_adapter : public testing::Test {
protected:
  context_type context;
  platform_type platform;

  auto SetUp() -> void override {
    platform.set_window_user_pointer(
        nullptr, static_cast<platform_type::user_pointer_type>(&context));
    platform.key_to_scancode[GLFW_KEY_ESCAPE] = 0x42;
  }
};

struct key_param {
  int glfw_key;
  int os_scancode;
  jage::input::keyboard::key expected_key;
  jage::input::keyboard::scan_code expected_scancode;
};

TEST_F(glfw_adapter, should_set_keyboard_callback) {
  std::ignore = adapter_type{nullptr, platform};
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS, 0);
  EXPECT_FALSE(std::empty(context.buffer));
}

class glfw_adapter_key_map : public testing::TestWithParam<key_param> {
protected:
  context_type context;
  platform_type platform;

  auto SetUp() -> void override {
    platform.set_window_user_pointer(
        nullptr, static_cast<platform_type::user_pointer_type>(&context));
    const auto &[glfw_key, os_scancode, expected_key, expected_scancode] =
        GetParam();
    platform.key_to_scancode[glfw_key] = os_scancode;
  }
};

TEST_P(glfw_adapter_key_map, should_map_glfw_keys_and_scan_codes) {
  std::ignore = adapter_type{nullptr, platform};
  const auto &[glfw_key, os_scancode, expected_key, expected_scancode] =
      GetParam();
  platform.trigger_key_callback(glfw_key, os_scancode, GLFW_PRESS, 0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front()));
  const auto event = std::get<keyboard_event>(context.buffer.front());
  EXPECT_EQ(expected_key, event.key);
  EXPECT_EQ(expected_scancode, event.scan_code);
}

INSTANTIATE_TEST_SUITE_P(
    key_mappings, glfw_adapter_key_map,
    testing::Values(
        key_param{
            .glfw_key = GLFW_KEY_ESCAPE,
            .os_scancode = 0x29,
            .expected_key = jage::input::keyboard::key::escape,
            .expected_scancode = jage::input::keyboard::scan_code::escape,
        },
        key_param{
            .glfw_key = GLFW_KEY_A,
            .os_scancode = 0x04,
            .expected_key = jage::input::keyboard::key::a,
            .expected_scancode = jage::input::keyboard::scan_code::a,
        }));