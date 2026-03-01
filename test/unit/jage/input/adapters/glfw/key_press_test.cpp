#include <jage/external/glfw.hpp>
#include <jage/input/adapters/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/keyboard/events/key_press.hpp>
#include <jage/time/durations.hpp>

#include <jage/test/fakes/input/context/glfw.hpp>
#include <jage/test/fakes/input/platforms/glfw.hpp>

#include <gtest/gtest.h>

#include <array>
#include <variant>

namespace context = jage::test::fakes::input::context;
namespace platforms = jage::test::fakes::input::platforms;
namespace keyboard = jage::input::keyboard;
namespace durations = jage::time::durations;
namespace adapters = jage::input::adapters;
using jage::input::modifier;

using keyboard_event = keyboard::events::key_press;
using context_type = context::glfw<durations::nanoseconds,
                                   jage::input::event<durations::nanoseconds>>;
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

  auto TearDown() -> void override { platform.reset(); }
};

TEST_F(glfw_adapter, should_set_keyboard_callback) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS, 0);
  EXPECT_FALSE(std::empty(context.buffer));
}

TEST_F(glfw_adapter,
       should_return_unidentified_for_key_that_is_not_identified) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  platform.trigger_key_callback(337, 0x05, GLFW_PRESS, 0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  const auto event = std::get<keyboard_event>(context.buffer.front().payload);
  EXPECT_EQ(keyboard::key::unidentified, event.key);
  EXPECT_EQ(keyboard::scancode::unidentified, event.scancode);
}

TEST_F(glfw_adapter,
       should_return_unidentified_for_scancode_that_is_not_identified) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, GLFW_KEY_UNKNOWN, GLFW_PRESS,
                                0);
  platform.trigger_key_callback(GLFW_KEY_UNKNOWN, 0x43, GLFW_PRESS, 0);

  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  {
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_EQ(keyboard::key::escape, event.key);
    EXPECT_EQ(keyboard::scancode::unidentified, event.scancode);
  }
  context.buffer.pop_front();
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  {
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_EQ(keyboard::key::unidentified, event.key);
    EXPECT_EQ(keyboard::scancode::unidentified, event.scancode);
  }
}

TEST_F(glfw_adapter, should_map_action_appropriately) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_RELEASE, 0);
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS, 0);
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_REPEAT, 0);
  ASSERT_EQ(3, std::size(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  {
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_EQ(keyboard::action::release, event.action);
  }
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.at(1).payload));
  {
    const auto event = std::get<keyboard_event>(context.buffer.at(1).payload);
    EXPECT_EQ(keyboard::action::press, event.action);
  }
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.back().payload));
  {
    const auto event = std::get<keyboard_event>(context.buffer.back().payload);
    EXPECT_EQ(keyboard::action::repeat, event.action);
  }
}

TEST_F(glfw_adapter, should_map_modifiers) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_SHIFT);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::left_shift))))
        << "Modifiers: " << event.modifiers.to_string();
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::right_shift))))
        << "Modifiers: " << event.modifiers.to_string();
  }
  context.buffer.clear();
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_ALT);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::left_alt))))
        << "Modifiers: " << event.modifiers.to_string();
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::right_alt))))
        << "Modifiers: " << event.modifiers.to_string();
  }
  context.buffer.clear();
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_CONTROL);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::left_control))))
        << "Modifiers: " << event.modifiers.to_string();
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::right_control))))
        << "Modifiers: " << event.modifiers.to_string();
  }
  context.buffer.clear();
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_SUPER);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::left_gui))))
        << "Modifiers: " << event.modifiers.to_string();
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::right_gui))))
        << "Modifiers: " << event.modifiers.to_string();
  }
  context.buffer.clear();
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_CAPS_LOCK);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::caps_lock))))
        << "Modifiers: " << event.modifiers.to_string();
  }
  context.buffer.clear();
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_NUM_LOCK);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    const auto event = std::get<keyboard_event>(context.buffer.front().payload);
    EXPECT_TRUE(event.modifiers.test(
        static_cast<std::size_t>(std::to_underlying(modifier::num_lock))))
        << "Modifiers: " << event.modifiers.to_string();
  }
}

TEST_F(glfw_adapter, should_set_timestamp) {
  platform.set_seconds_since_init(platform_type::duration_type{11});
  EXPECT_EQ(platform_type::duration_type{11},
            platform.get_seconds_since_init());
  adapter_type::initialize(nullptr, platform,
                           context_type::duration_type{42e+9});
  EXPECT_EQ(jage::time::cast<platform_type::duration_type>(
                context_type::duration_type{42e+9}),
            platform.get_seconds_since_init());
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_SHIFT);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    EXPECT_EQ(context_type::duration_type{42e+9}, context.buffer.front().timestamp);
  }
  context.buffer.clear();
  platform.set_seconds_since_init(context_type::duration_type{99e+9});
  platform.trigger_key_callback(GLFW_KEY_ESCAPE, 0x04, GLFW_PRESS,
                                GLFW_MOD_SHIFT);
  {
    ASSERT_FALSE(std::empty(context.buffer));
    ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
    EXPECT_EQ(context_type::duration_type{99e+9}, context.buffer.front().timestamp);
  }
}

TEST_F(glfw_adapter, should_map_unknown_to_unidentified) {
  platform.set_seconds_since_init(platform_type::duration_type{11});
  EXPECT_EQ(platform_type::duration_type{11},
            platform.get_seconds_since_init());
  adapter_type::initialize(nullptr, platform,
                           context_type::duration_type{42e+9});
  EXPECT_EQ(jage::time::cast<platform_type::duration_type>(
                context_type::duration_type{42e+9}),
            platform.get_seconds_since_init());
  platform.trigger_key_callback(GLFW_KEY_UNKNOWN, 0x04, GLFW_PRESS,
                                GLFW_MOD_SHIFT);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  const auto event = std::get<keyboard_event>(context.buffer.front().payload);
  EXPECT_EQ(jage::input::keyboard::key::unidentified, event.key);
}

struct key_param {
  int glfw_key;
  int os_scancode;
  jage::input::keyboard::key expected_key;
  jage::input::keyboard::scancode expected_scancode;
};

constexpr auto
make_key_param(const int glfw_key, const keyboard::key expected_key,
               const keyboard::scancode expected_scancode) -> key_param {
  return key_param{
      .glfw_key = glfw_key,
      .os_scancode = static_cast<int>(expected_scancode),
      .expected_key = expected_key,
      .expected_scancode = expected_scancode,
  };
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
    ASSERT_EQ(1, std::size(platform.key_to_scancode));
  }

  auto TearDown() -> void override { platform.reset(); }
};

TEST_P(glfw_adapter_key_map, should_map_glfw_keys_and_scan_codes) {
  adapter_type::initialize(nullptr, platform,
                           platform_type::context_type::duration_type{});
  const auto &[glfw_key, os_scancode, expected_key, expected_scancode] =
      GetParam();
  platform.trigger_key_callback(glfw_key, os_scancode, GLFW_PRESS, 0);
  ASSERT_FALSE(std::empty(context.buffer));
  ASSERT_TRUE(std::holds_alternative<keyboard_event>(context.buffer.front().payload));
  const auto event = std::get<keyboard_event>(context.buffer.front().payload);
  EXPECT_EQ(expected_key, event.key);
  EXPECT_EQ(expected_scancode, event.scancode);
}

INSTANTIATE_TEST_SUITE_P(
    key_mappings, glfw_adapter_key_map,
    testing::ValuesIn(std::to_array<key_param>({
        make_key_param(GLFW_KEY_SPACE, keyboard::key::spacebar,
                       keyboard::scancode::spacebar),
        make_key_param(GLFW_KEY_APOSTROPHE, keyboard::key::apostrophe,
                       keyboard::scancode::apostrophe),
        make_key_param(GLFW_KEY_COMMA, keyboard::key::comma,
                       keyboard::scancode::comma),
        make_key_param(GLFW_KEY_MINUS, keyboard::key::minus,
                       keyboard::scancode::minus),
        make_key_param(GLFW_KEY_PERIOD, keyboard::key::period,
                       keyboard::scancode::period),
        make_key_param(GLFW_KEY_SLASH, keyboard::key::slash,
                       keyboard::scancode::slash),
        make_key_param(GLFW_KEY_0, keyboard::key::_0, keyboard::scancode::_0),
        make_key_param(GLFW_KEY_1, keyboard::key::_1, keyboard::scancode::_1),
        make_key_param(GLFW_KEY_2, keyboard::key::_2, keyboard::scancode::_2),
        make_key_param(GLFW_KEY_3, keyboard::key::_3, keyboard::scancode::_3),
        make_key_param(GLFW_KEY_4, keyboard::key::_4, keyboard::scancode::_4),
        make_key_param(GLFW_KEY_5, keyboard::key::_5, keyboard::scancode::_5),
        make_key_param(GLFW_KEY_6, keyboard::key::_6, keyboard::scancode::_6),
        make_key_param(GLFW_KEY_7, keyboard::key::_7, keyboard::scancode::_7),
        make_key_param(GLFW_KEY_8, keyboard::key::_8, keyboard::scancode::_8),
        make_key_param(GLFW_KEY_9, keyboard::key::_9, keyboard::scancode::_9),
        make_key_param(GLFW_KEY_SEMICOLON, keyboard::key::semicolon,
                       keyboard::scancode::semicolon),
        make_key_param(GLFW_KEY_EQUAL, keyboard::key::equal,
                       keyboard::scancode::equal),
        make_key_param(GLFW_KEY_A, keyboard::key::a, keyboard::scancode::a),
        make_key_param(GLFW_KEY_B, keyboard::key::b, keyboard::scancode::b),
        make_key_param(GLFW_KEY_C, keyboard::key::c, keyboard::scancode::c),
        make_key_param(GLFW_KEY_D, keyboard::key::d, keyboard::scancode::d),
        make_key_param(GLFW_KEY_E, keyboard::key::e, keyboard::scancode::e),
        make_key_param(GLFW_KEY_F, keyboard::key::f, keyboard::scancode::f),
        make_key_param(GLFW_KEY_G, keyboard::key::g, keyboard::scancode::g),
        make_key_param(GLFW_KEY_H, keyboard::key::h, keyboard::scancode::h),
        make_key_param(GLFW_KEY_I, keyboard::key::i, keyboard::scancode::i),
        make_key_param(GLFW_KEY_J, keyboard::key::j, keyboard::scancode::j),
        make_key_param(GLFW_KEY_K, keyboard::key::k, keyboard::scancode::k),
        make_key_param(GLFW_KEY_L, keyboard::key::l, keyboard::scancode::l),
        make_key_param(GLFW_KEY_M, keyboard::key::m, keyboard::scancode::m),
        make_key_param(GLFW_KEY_N, keyboard::key::n, keyboard::scancode::n),
        make_key_param(GLFW_KEY_O, keyboard::key::o, keyboard::scancode::o),
        make_key_param(GLFW_KEY_P, keyboard::key::p, keyboard::scancode::p),
        make_key_param(GLFW_KEY_Q, keyboard::key::q, keyboard::scancode::q),
        make_key_param(GLFW_KEY_R, keyboard::key::r, keyboard::scancode::r),
        make_key_param(GLFW_KEY_S, keyboard::key::s, keyboard::scancode::s),
        make_key_param(GLFW_KEY_T, keyboard::key::t, keyboard::scancode::t),
        make_key_param(GLFW_KEY_U, keyboard::key::u, keyboard::scancode::u),
        make_key_param(GLFW_KEY_V, keyboard::key::v, keyboard::scancode::v),
        make_key_param(GLFW_KEY_W, keyboard::key::w, keyboard::scancode::w),
        make_key_param(GLFW_KEY_X, keyboard::key::x, keyboard::scancode::x),
        make_key_param(GLFW_KEY_Y, keyboard::key::y, keyboard::scancode::y),
        make_key_param(GLFW_KEY_Z, keyboard::key::z, keyboard::scancode::z),
        make_key_param(GLFW_KEY_LEFT_BRACKET, keyboard::key::left_bracket,
                       keyboard::scancode::left_bracket),
        make_key_param(GLFW_KEY_BACKSLASH, keyboard::key::backslash,
                       keyboard::scancode::backslash),
        make_key_param(GLFW_KEY_RIGHT_BRACKET, keyboard::key::right_bracket,
                       keyboard::scancode::right_bracket),
        make_key_param(GLFW_KEY_GRAVE_ACCENT, keyboard::key::grave_accent,
                       keyboard::scancode::grave_accent),
        make_key_param(GLFW_KEY_WORLD_1, keyboard::key::world_1,
                       keyboard::scancode::world_1),
        make_key_param(GLFW_KEY_WORLD_2, keyboard::key::world_2,
                       keyboard::scancode::world_2),
        make_key_param(GLFW_KEY_ESCAPE, keyboard::key::escape,
                       keyboard::scancode::escape),
        make_key_param(GLFW_KEY_ENTER, keyboard::key::enter,
                       keyboard::scancode::enter),
        make_key_param(GLFW_KEY_TAB, keyboard::key::tab,
                       keyboard::scancode::tab),
        make_key_param(GLFW_KEY_BACKSPACE, keyboard::key::backspace,
                       keyboard::scancode::backspace),
        make_key_param(GLFW_KEY_INSERT, keyboard::key::insert,
                       keyboard::scancode::insert),
        make_key_param(GLFW_KEY_DELETE, keyboard::key::delete_key,
                       keyboard::scancode::delete_key),
        make_key_param(GLFW_KEY_RIGHT, keyboard::key::arrow_right,
                       keyboard::scancode::arrow_right),
        make_key_param(GLFW_KEY_LEFT, keyboard::key::arrow_left,
                       keyboard::scancode::arrow_left),
        make_key_param(GLFW_KEY_DOWN, keyboard::key::arrow_down,
                       keyboard::scancode::arrow_down),
        make_key_param(GLFW_KEY_UP, keyboard::key::arrow_up,
                       keyboard::scancode::arrow_up),
        make_key_param(GLFW_KEY_PAGE_UP, keyboard::key::page_up,
                       keyboard::scancode::page_up),
        make_key_param(GLFW_KEY_PAGE_DOWN, keyboard::key::page_down,
                       keyboard::scancode::page_down),
        make_key_param(GLFW_KEY_HOME, keyboard::key::home,
                       keyboard::scancode::home),
        make_key_param(GLFW_KEY_END, keyboard::key::end,
                       keyboard::scancode::end),
        make_key_param(GLFW_KEY_CAPS_LOCK, keyboard::key::caps_lock,
                       keyboard::scancode::caps_lock),
        make_key_param(GLFW_KEY_SCROLL_LOCK, keyboard::key::scroll_lock,
                       keyboard::scancode::scroll_lock),
        make_key_param(GLFW_KEY_NUM_LOCK, keyboard::key::num_lock,
                       keyboard::scancode::num_lock),
        make_key_param(GLFW_KEY_PRINT_SCREEN, keyboard::key::print_screen,
                       keyboard::scancode::print_screen),
        make_key_param(GLFW_KEY_PAUSE, keyboard::key::pause,
                       keyboard::scancode::pause),
        make_key_param(GLFW_KEY_F1, keyboard::key::F1, keyboard::scancode::F1),
        make_key_param(GLFW_KEY_F2, keyboard::key::F2, keyboard::scancode::F2),
        make_key_param(GLFW_KEY_F3, keyboard::key::F3, keyboard::scancode::F3),
        make_key_param(GLFW_KEY_F4, keyboard::key::F4, keyboard::scancode::F4),
        make_key_param(GLFW_KEY_F5, keyboard::key::F5, keyboard::scancode::F5),
        make_key_param(GLFW_KEY_F6, keyboard::key::F6, keyboard::scancode::F6),
        make_key_param(GLFW_KEY_F7, keyboard::key::F7, keyboard::scancode::F7),
        make_key_param(GLFW_KEY_F8, keyboard::key::F8, keyboard::scancode::F8),
        make_key_param(GLFW_KEY_F9, keyboard::key::F9, keyboard::scancode::F9),
        make_key_param(GLFW_KEY_F10, keyboard::key::F10,
                       keyboard::scancode::F10),
        make_key_param(GLFW_KEY_F11, keyboard::key::F11,
                       keyboard::scancode::F11),
        make_key_param(GLFW_KEY_F12, keyboard::key::F12,
                       keyboard::scancode::F12),
        make_key_param(GLFW_KEY_F13, keyboard::key::F13,
                       keyboard::scancode::F13),
        make_key_param(GLFW_KEY_F14, keyboard::key::F14,
                       keyboard::scancode::F14),
        make_key_param(GLFW_KEY_F15, keyboard::key::F15,
                       keyboard::scancode::F15),
        make_key_param(GLFW_KEY_F16, keyboard::key::F16,
                       keyboard::scancode::F16),
        make_key_param(GLFW_KEY_F17, keyboard::key::F17,
                       keyboard::scancode::F17),
        make_key_param(GLFW_KEY_F18, keyboard::key::F18,
                       keyboard::scancode::F18),
        make_key_param(GLFW_KEY_F19, keyboard::key::F19,
                       keyboard::scancode::F19),
        make_key_param(GLFW_KEY_F20, keyboard::key::F20,
                       keyboard::scancode::F20),
        make_key_param(GLFW_KEY_F21, keyboard::key::F21,
                       keyboard::scancode::F21),
        make_key_param(GLFW_KEY_F22, keyboard::key::F22,
                       keyboard::scancode::F22),
        make_key_param(GLFW_KEY_F23, keyboard::key::F23,
                       keyboard::scancode::F23),
        make_key_param(GLFW_KEY_F24, keyboard::key::F24,
                       keyboard::scancode::F24),
        make_key_param(GLFW_KEY_F25, keyboard::key::execute,
                       keyboard::scancode::execute),
        make_key_param(GLFW_KEY_KP_0, keyboard::key::kp_0,
                       keyboard::scancode::kp_0),
        make_key_param(GLFW_KEY_KP_1, keyboard::key::kp_1,
                       keyboard::scancode::kp_1),
        make_key_param(GLFW_KEY_KP_2, keyboard::key::kp_2,
                       keyboard::scancode::kp_2),
        make_key_param(GLFW_KEY_KP_3, keyboard::key::kp_3,
                       keyboard::scancode::kp_3),
        make_key_param(GLFW_KEY_KP_4, keyboard::key::kp_4,
                       keyboard::scancode::kp_4),
        make_key_param(GLFW_KEY_KP_5, keyboard::key::kp_5,
                       keyboard::scancode::kp_5),
        make_key_param(GLFW_KEY_KP_6, keyboard::key::kp_6,
                       keyboard::scancode::kp_6),
        make_key_param(GLFW_KEY_KP_7, keyboard::key::kp_7,
                       keyboard::scancode::kp_7),
        make_key_param(GLFW_KEY_KP_8, keyboard::key::kp_8,
                       keyboard::scancode::kp_8),
        make_key_param(GLFW_KEY_KP_9, keyboard::key::kp_9,
                       keyboard::scancode::kp_9),
        make_key_param(GLFW_KEY_KP_DECIMAL, keyboard::key::kp_decimal,
                       keyboard::scancode::kp_decimal),
        make_key_param(GLFW_KEY_KP_DIVIDE, keyboard::key::kp_divide,
                       keyboard::scancode::kp_divide),
        make_key_param(GLFW_KEY_KP_MULTIPLY, keyboard::key::kp_multiply,
                       keyboard::scancode::kp_multiply),
        make_key_param(GLFW_KEY_KP_SUBTRACT, keyboard::key::kp_subtract,
                       keyboard::scancode::kp_subtract),
        make_key_param(GLFW_KEY_KP_ADD, keyboard::key::kp_add,
                       keyboard::scancode::kp_add),
        make_key_param(GLFW_KEY_KP_ENTER, keyboard::key::kp_enter,
                       keyboard::scancode::kp_enter),
        make_key_param(GLFW_KEY_KP_EQUAL, keyboard::key::kp_equal,
                       keyboard::scancode::kp_equal),
        make_key_param(GLFW_KEY_LEFT_SHIFT, keyboard::key::left_shift,
                       keyboard::scancode::left_shift),
        make_key_param(GLFW_KEY_LEFT_CONTROL, keyboard::key::left_control,
                       keyboard::scancode::left_control),
        make_key_param(GLFW_KEY_LEFT_ALT, keyboard::key::left_alt,
                       keyboard::scancode::left_alt),
        make_key_param(GLFW_KEY_LEFT_SUPER, keyboard::key::left_super,
                       keyboard::scancode::left_super),
        make_key_param(GLFW_KEY_RIGHT_SHIFT, keyboard::key::right_shift,
                       keyboard::scancode::right_shift),
        make_key_param(GLFW_KEY_RIGHT_CONTROL, keyboard::key::right_control,
                       keyboard::scancode::right_control),
        make_key_param(GLFW_KEY_RIGHT_ALT, keyboard::key::right_alt,
                       keyboard::scancode::right_alt),
        make_key_param(GLFW_KEY_RIGHT_SUPER, keyboard::key::right_super,
                       keyboard::scancode::right_super),
        make_key_param(GLFW_KEY_MENU, keyboard::key::menu,
                       keyboard::scancode::menu),
    })));