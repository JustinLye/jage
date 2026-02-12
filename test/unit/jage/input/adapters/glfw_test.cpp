#include <jage/input/adapters/glfw.hpp>
#include <jage/input/keyboard/event.hpp>
#include <jage/time/durations.hpp>

#include <GLFW/glfw3.h>

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

constexpr auto
make_key_param(const int glfw_key, const keyboard::key expected_key,
               const keyboard::scan_code expected_scancode) -> key_param {
  return key_param{
      .glfw_key = glfw_key,
      .os_scancode = static_cast<int>(expected_scancode),
      .expected_key = expected_key,
      .expected_scancode = expected_scancode,
  };
}

constexpr auto key_params = std::to_array<key_param>({
    make_key_param(GLFW_KEY_SPACE, keyboard::key::spacebar,
                   keyboard::scan_code::spacebar),
    make_key_param(GLFW_KEY_APOSTROPHE, keyboard::key::apostrophe,
                   keyboard::scan_code::apostrophe),
    make_key_param(GLFW_KEY_COMMA, keyboard::key::comma,
                   keyboard::scan_code::comma),
    make_key_param(GLFW_KEY_MINUS, keyboard::key::minus,
                   keyboard::scan_code::minus),
    make_key_param(GLFW_KEY_PERIOD, keyboard::key::period,
                   keyboard::scan_code::period),
    make_key_param(GLFW_KEY_SLASH, keyboard::key::slash,
                   keyboard::scan_code::slash),
    make_key_param(GLFW_KEY_0, keyboard::key::_0, keyboard::scan_code::_0),
    make_key_param(GLFW_KEY_1, keyboard::key::_1, keyboard::scan_code::_1),
    make_key_param(GLFW_KEY_2, keyboard::key::_2, keyboard::scan_code::_2),
    make_key_param(GLFW_KEY_3, keyboard::key::_3, keyboard::scan_code::_3),
    make_key_param(GLFW_KEY_4, keyboard::key::_4, keyboard::scan_code::_4),
    make_key_param(GLFW_KEY_5, keyboard::key::_5, keyboard::scan_code::_5),
    make_key_param(GLFW_KEY_6, keyboard::key::_6, keyboard::scan_code::_6),
    make_key_param(GLFW_KEY_7, keyboard::key::_7, keyboard::scan_code::_7),
    make_key_param(GLFW_KEY_8, keyboard::key::_8, keyboard::scan_code::_8),
    make_key_param(GLFW_KEY_9, keyboard::key::_9, keyboard::scan_code::_9),
    make_key_param(GLFW_KEY_SEMICOLON, keyboard::key::semicolon,
                   keyboard::scan_code::semicolon),
    make_key_param(GLFW_KEY_EQUAL, keyboard::key::equal,
                   keyboard::scan_code::equal),
    make_key_param(GLFW_KEY_A, keyboard::key::a, keyboard::scan_code::a),
    make_key_param(GLFW_KEY_B, keyboard::key::b, keyboard::scan_code::b),
    make_key_param(GLFW_KEY_C, keyboard::key::c, keyboard::scan_code::c),
    make_key_param(GLFW_KEY_D, keyboard::key::d, keyboard::scan_code::d),
    make_key_param(GLFW_KEY_E, keyboard::key::e, keyboard::scan_code::e),
    make_key_param(GLFW_KEY_F, keyboard::key::f, keyboard::scan_code::f),
    make_key_param(GLFW_KEY_G, keyboard::key::g, keyboard::scan_code::g),
    make_key_param(GLFW_KEY_H, keyboard::key::h, keyboard::scan_code::h),
    make_key_param(GLFW_KEY_I, keyboard::key::i, keyboard::scan_code::i),
    make_key_param(GLFW_KEY_J, keyboard::key::j, keyboard::scan_code::j),
    make_key_param(GLFW_KEY_K, keyboard::key::k, keyboard::scan_code::k),
    make_key_param(GLFW_KEY_L, keyboard::key::l, keyboard::scan_code::l),
    make_key_param(GLFW_KEY_M, keyboard::key::m, keyboard::scan_code::m),
    make_key_param(GLFW_KEY_N, keyboard::key::n, keyboard::scan_code::n),
    make_key_param(GLFW_KEY_O, keyboard::key::o, keyboard::scan_code::o),
    make_key_param(GLFW_KEY_P, keyboard::key::p, keyboard::scan_code::p),
    make_key_param(GLFW_KEY_Q, keyboard::key::q, keyboard::scan_code::q),
    make_key_param(GLFW_KEY_R, keyboard::key::r, keyboard::scan_code::r),
    make_key_param(GLFW_KEY_S, keyboard::key::s, keyboard::scan_code::s),
    make_key_param(GLFW_KEY_T, keyboard::key::t, keyboard::scan_code::t),
    make_key_param(GLFW_KEY_U, keyboard::key::u, keyboard::scan_code::u),
    make_key_param(GLFW_KEY_V, keyboard::key::v, keyboard::scan_code::v),
    make_key_param(GLFW_KEY_W, keyboard::key::w, keyboard::scan_code::w),
    make_key_param(GLFW_KEY_X, keyboard::key::x, keyboard::scan_code::x),
    make_key_param(GLFW_KEY_Y, keyboard::key::y, keyboard::scan_code::y),
    make_key_param(GLFW_KEY_Z, keyboard::key::z, keyboard::scan_code::z),
    make_key_param(GLFW_KEY_LEFT_BRACKET, keyboard::key::left_bracket,
                   keyboard::scan_code::left_bracket),
    make_key_param(GLFW_KEY_BACKSLASH, keyboard::key::backslash,
                   keyboard::scan_code::backslash),
    make_key_param(GLFW_KEY_RIGHT_BRACKET, keyboard::key::right_bracket,
                   keyboard::scan_code::right_bracket),
    make_key_param(GLFW_KEY_GRAVE_ACCENT, keyboard::key::grave_accent,
                   keyboard::scan_code::grave_accent),
    make_key_param(GLFW_KEY_WORLD_1, keyboard::key::world_1,
                   keyboard::scan_code::world_1),
    make_key_param(GLFW_KEY_WORLD_2, keyboard::key::world_2,
                   keyboard::scan_code::world_2),
    make_key_param(GLFW_KEY_ESCAPE, keyboard::key::escape,
                   keyboard::scan_code::escape),
    make_key_param(GLFW_KEY_ENTER, keyboard::key::enter,
                   keyboard::scan_code::enter),
    make_key_param(GLFW_KEY_TAB, keyboard::key::tab, keyboard::scan_code::tab),
    make_key_param(GLFW_KEY_BACKSPACE, keyboard::key::backspace,
                   keyboard::scan_code::backspace),
    make_key_param(GLFW_KEY_INSERT, keyboard::key::insert,
                   keyboard::scan_code::insert),
    make_key_param(GLFW_KEY_DELETE, keyboard::key::delete_key,
                   keyboard::scan_code::delete_key),
    make_key_param(GLFW_KEY_RIGHT, keyboard::key::arrow_right,
                   keyboard::scan_code::arrow_right),
    make_key_param(GLFW_KEY_LEFT, keyboard::key::arrow_left,
                   keyboard::scan_code::arrow_left),
    make_key_param(GLFW_KEY_DOWN, keyboard::key::arrow_down,
                   keyboard::scan_code::arrow_down),
    make_key_param(GLFW_KEY_UP, keyboard::key::arrow_up,
                   keyboard::scan_code::arrow_up),
    make_key_param(GLFW_KEY_PAGE_UP, keyboard::key::page_up,
                   keyboard::scan_code::page_up),
    make_key_param(GLFW_KEY_PAGE_DOWN, keyboard::key::page_down,
                   keyboard::scan_code::page_down),
    make_key_param(GLFW_KEY_HOME, keyboard::key::home,
                   keyboard::scan_code::home),
    make_key_param(GLFW_KEY_END, keyboard::key::end, keyboard::scan_code::end),
    make_key_param(GLFW_KEY_CAPS_LOCK, keyboard::key::caps_lock,
                   keyboard::scan_code::caps_lock),
    make_key_param(GLFW_KEY_SCROLL_LOCK, keyboard::key::scroll_lock,
                   keyboard::scan_code::scroll_lock),
    make_key_param(GLFW_KEY_NUM_LOCK, keyboard::key::num_lock,
                   keyboard::scan_code::num_lock),
    make_key_param(GLFW_KEY_PRINT_SCREEN, keyboard::key::print_screen,
                   keyboard::scan_code::print_screen),
    make_key_param(GLFW_KEY_PAUSE, keyboard::key::pause,
                   keyboard::scan_code::pause),
    make_key_param(GLFW_KEY_F1, keyboard::key::F1, keyboard::scan_code::F1),
    make_key_param(GLFW_KEY_F2, keyboard::key::F2, keyboard::scan_code::F2),
    make_key_param(GLFW_KEY_F3, keyboard::key::F3, keyboard::scan_code::F3),
    make_key_param(GLFW_KEY_F4, keyboard::key::F4, keyboard::scan_code::F4),
    make_key_param(GLFW_KEY_F5, keyboard::key::F5, keyboard::scan_code::F5),
    make_key_param(GLFW_KEY_F6, keyboard::key::F6, keyboard::scan_code::F6),
    make_key_param(GLFW_KEY_F7, keyboard::key::F7, keyboard::scan_code::F7),
    make_key_param(GLFW_KEY_F8, keyboard::key::F8, keyboard::scan_code::F8),
    make_key_param(GLFW_KEY_F9, keyboard::key::F9, keyboard::scan_code::F9),
    make_key_param(GLFW_KEY_F10, keyboard::key::F10, keyboard::scan_code::F10),
    make_key_param(GLFW_KEY_F11, keyboard::key::F11, keyboard::scan_code::F11),
    make_key_param(GLFW_KEY_F12, keyboard::key::F12, keyboard::scan_code::F12),
    make_key_param(GLFW_KEY_F13, keyboard::key::F13, keyboard::scan_code::F13),
    make_key_param(GLFW_KEY_F14, keyboard::key::F14, keyboard::scan_code::F14),
    make_key_param(GLFW_KEY_F15, keyboard::key::F15, keyboard::scan_code::F15),
    make_key_param(GLFW_KEY_F16, keyboard::key::F16, keyboard::scan_code::F16),
    make_key_param(GLFW_KEY_F17, keyboard::key::F17, keyboard::scan_code::F17),
    make_key_param(GLFW_KEY_F18, keyboard::key::F18, keyboard::scan_code::F18),
    make_key_param(GLFW_KEY_F19, keyboard::key::F19, keyboard::scan_code::F19),
    make_key_param(GLFW_KEY_F20, keyboard::key::F20, keyboard::scan_code::F20),
    make_key_param(GLFW_KEY_F21, keyboard::key::F21, keyboard::scan_code::F21),
    make_key_param(GLFW_KEY_F22, keyboard::key::F22, keyboard::scan_code::F22),
    make_key_param(GLFW_KEY_F23, keyboard::key::F23, keyboard::scan_code::F23),
    make_key_param(GLFW_KEY_F24, keyboard::key::F24, keyboard::scan_code::F24),
    make_key_param(GLFW_KEY_F25, keyboard::key::execute,
                   keyboard::scan_code::execute),
    make_key_param(GLFW_KEY_KP_0, keyboard::key::kp_0,
                   keyboard::scan_code::kp_0),
    make_key_param(GLFW_KEY_KP_1, keyboard::key::kp_1,
                   keyboard::scan_code::kp_1),
    make_key_param(GLFW_KEY_KP_2, keyboard::key::kp_2,
                   keyboard::scan_code::kp_2),
    make_key_param(GLFW_KEY_KP_3, keyboard::key::kp_3,
                   keyboard::scan_code::kp_3),
    make_key_param(GLFW_KEY_KP_4, keyboard::key::kp_4,
                   keyboard::scan_code::kp_4),
    make_key_param(GLFW_KEY_KP_5, keyboard::key::kp_5,
                   keyboard::scan_code::kp_5),
    make_key_param(GLFW_KEY_KP_6, keyboard::key::kp_6,
                   keyboard::scan_code::kp_6),
    make_key_param(GLFW_KEY_KP_7, keyboard::key::kp_7,
                   keyboard::scan_code::kp_7),
    make_key_param(GLFW_KEY_KP_8, keyboard::key::kp_8,
                   keyboard::scan_code::kp_8),
    make_key_param(GLFW_KEY_KP_9, keyboard::key::kp_9,
                   keyboard::scan_code::kp_9),
    make_key_param(GLFW_KEY_KP_DECIMAL, keyboard::key::kp_decimal,
                   keyboard::scan_code::kp_decimal),
    make_key_param(GLFW_KEY_KP_DIVIDE, keyboard::key::kp_divide,
                   keyboard::scan_code::kp_divide),
    make_key_param(GLFW_KEY_KP_MULTIPLY, keyboard::key::kp_multiply,
                   keyboard::scan_code::kp_multiply),
    make_key_param(GLFW_KEY_KP_SUBTRACT, keyboard::key::kp_subtract,
                   keyboard::scan_code::kp_subtract),
    make_key_param(GLFW_KEY_KP_ADD, keyboard::key::kp_add,
                   keyboard::scan_code::kp_add),
    make_key_param(GLFW_KEY_KP_ENTER, keyboard::key::kp_enter,
                   keyboard::scan_code::kp_enter),
    make_key_param(GLFW_KEY_KP_EQUAL, keyboard::key::kp_equal,
                   keyboard::scan_code::kp_equal),
    make_key_param(GLFW_KEY_LEFT_SHIFT, keyboard::key::left_shift,
                   keyboard::scan_code::left_shift),
    make_key_param(GLFW_KEY_LEFT_CONTROL, keyboard::key::left_control,
                   keyboard::scan_code::left_control),
    make_key_param(GLFW_KEY_LEFT_ALT, keyboard::key::left_alt,
                   keyboard::scan_code::left_alt),
    make_key_param(GLFW_KEY_LEFT_SUPER, keyboard::key::left_super,
                   keyboard::scan_code::left_super),
    make_key_param(GLFW_KEY_RIGHT_SHIFT, keyboard::key::right_shift,
                   keyboard::scan_code::right_shift),
    make_key_param(GLFW_KEY_RIGHT_CONTROL, keyboard::key::right_control,
                   keyboard::scan_code::right_control),
    make_key_param(GLFW_KEY_RIGHT_ALT, keyboard::key::right_alt,
                   keyboard::scan_code::right_alt),
    make_key_param(GLFW_KEY_RIGHT_SUPER, keyboard::key::right_super,
                   keyboard::scan_code::right_super),
    make_key_param(GLFW_KEY_MENU, keyboard::key::menu,
                   keyboard::scan_code::menu),
});

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

INSTANTIATE_TEST_SUITE_P(key_mappings, glfw_adapter_key_map,
                         testing::ValuesIn(key_params));
