#pragma once

#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/event.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/scan_code.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <vector>

namespace jage::input::adapters {
// TODO: Make this a concept
template <class TPlatform> class glfw {

  static std::array<keyboard::key, GLFW_KEY_LAST + 1> logical_keys_;
  static std::vector<keyboard::scan_code> physical_keys_;

  static constexpr auto get_physical_key =
      [](const int scancode) -> keyboard::scan_code {
    if (0 > scancode) [[unlikely]] {
      return keyboard::scan_code::unidentified;
    } else if (static_cast<std::size_t>(scancode) >= std::size(physical_keys_))
        [[unlikely]] {
      return keyboard::scan_code::unidentified;
    } else [[likely]] {
      return physical_keys_[static_cast<std::size_t>(scancode)];
    }
  };

  static constexpr auto key_callback =
      [](typename TPlatform::window_handler_pointer_type window, int key,
         int scancode, int, int) -> void {
    auto &context = *static_cast<typename TPlatform::context_type *>(
        TPlatform::get_window_user_pointer(window));
    context.push(typename TPlatform::context_type::event_type{
        keyboard::event<typename TPlatform::context_type::duration_type>{
            .timestamp = typename TPlatform::context_type::duration_type{},
            .key = logical_keys_[key],
            .scan_code = get_physical_key(scancode),
            .action = keyboard::action::press,
            .modifiers = 0,
        }});
  };

  static constexpr auto load_physical_scancodes =
      [](const TPlatform &platform) -> void {
    auto bind = [&](const auto glfw_key,
                    const keyboard::scan_code scancode) -> void {
      if (const auto os_scancode = platform.get_key_scancode(glfw_key);
          os_scancode != -1) {
        if (static_cast<std::size_t>(os_scancode) >=
            std::size(physical_keys_)) {
          physical_keys_.resize(static_cast<std::size_t>(os_scancode) + 1UZ,
                                keyboard::scan_code::unidentified);
        }
        physical_keys_[os_scancode] = scancode;
      }
    };

    bind(GLFW_KEY_SPACE, keyboard::scan_code::spacebar);
    bind(GLFW_KEY_APOSTROPHE, keyboard::scan_code::apostrophe);
    bind(GLFW_KEY_COMMA, keyboard::scan_code::comma);
    bind(GLFW_KEY_MINUS, keyboard::scan_code::minus);
    bind(GLFW_KEY_PERIOD, keyboard::scan_code::period);
    bind(GLFW_KEY_SLASH, keyboard::scan_code::slash);
    bind(GLFW_KEY_0, keyboard::scan_code::_0);
    bind(GLFW_KEY_1, keyboard::scan_code::_1);
    bind(GLFW_KEY_2, keyboard::scan_code::_2);
    bind(GLFW_KEY_3, keyboard::scan_code::_3);
    bind(GLFW_KEY_4, keyboard::scan_code::_4);
    bind(GLFW_KEY_5, keyboard::scan_code::_5);
    bind(GLFW_KEY_6, keyboard::scan_code::_6);
    bind(GLFW_KEY_7, keyboard::scan_code::_7);
    bind(GLFW_KEY_8, keyboard::scan_code::_8);
    bind(GLFW_KEY_9, keyboard::scan_code::_9);
    bind(GLFW_KEY_SEMICOLON, keyboard::scan_code::semicolon);
    bind(GLFW_KEY_EQUAL, keyboard::scan_code::equal);
    bind(GLFW_KEY_A, keyboard::scan_code::a);
    bind(GLFW_KEY_B, keyboard::scan_code::b);
    bind(GLFW_KEY_C, keyboard::scan_code::c);
    bind(GLFW_KEY_D, keyboard::scan_code::d);
    bind(GLFW_KEY_E, keyboard::scan_code::e);
    bind(GLFW_KEY_F, keyboard::scan_code::f);
    bind(GLFW_KEY_G, keyboard::scan_code::g);
    bind(GLFW_KEY_H, keyboard::scan_code::h);
    bind(GLFW_KEY_I, keyboard::scan_code::i);
    bind(GLFW_KEY_J, keyboard::scan_code::j);
    bind(GLFW_KEY_K, keyboard::scan_code::k);
    bind(GLFW_KEY_L, keyboard::scan_code::l);
    bind(GLFW_KEY_M, keyboard::scan_code::m);
    bind(GLFW_KEY_N, keyboard::scan_code::n);
    bind(GLFW_KEY_O, keyboard::scan_code::o);
    bind(GLFW_KEY_P, keyboard::scan_code::p);
    bind(GLFW_KEY_Q, keyboard::scan_code::q);
    bind(GLFW_KEY_R, keyboard::scan_code::r);
    bind(GLFW_KEY_S, keyboard::scan_code::s);
    bind(GLFW_KEY_T, keyboard::scan_code::t);
    bind(GLFW_KEY_U, keyboard::scan_code::u);
    bind(GLFW_KEY_V, keyboard::scan_code::v);
    bind(GLFW_KEY_W, keyboard::scan_code::w);
    bind(GLFW_KEY_X, keyboard::scan_code::x);
    bind(GLFW_KEY_Y, keyboard::scan_code::y);
    bind(GLFW_KEY_Z, keyboard::scan_code::z);
    bind(GLFW_KEY_LEFT_BRACKET, keyboard::scan_code::left_bracket);
    bind(GLFW_KEY_BACKSLASH, keyboard::scan_code::backslash);
    bind(GLFW_KEY_RIGHT_BRACKET, keyboard::scan_code::right_bracket);
    bind(GLFW_KEY_GRAVE_ACCENT, keyboard::scan_code::grave_accent);
    bind(GLFW_KEY_WORLD_1, keyboard::scan_code::world_1);
    bind(GLFW_KEY_WORLD_2, keyboard::scan_code::world_2);
    bind(GLFW_KEY_ESCAPE, keyboard::scan_code::escape);
    bind(GLFW_KEY_ENTER, keyboard::scan_code::enter);
    bind(GLFW_KEY_TAB, keyboard::scan_code::tab);
    bind(GLFW_KEY_BACKSPACE, keyboard::scan_code::backspace);
    bind(GLFW_KEY_INSERT, keyboard::scan_code::insert);
    bind(GLFW_KEY_DELETE, keyboard::scan_code::delete_key);
    bind(GLFW_KEY_RIGHT, keyboard::scan_code::arrow_right);
    bind(GLFW_KEY_LEFT, keyboard::scan_code::arrow_left);
    bind(GLFW_KEY_DOWN, keyboard::scan_code::arrow_down);
    bind(GLFW_KEY_UP, keyboard::scan_code::arrow_up);
    bind(GLFW_KEY_PAGE_UP, keyboard::scan_code::page_up);
    bind(GLFW_KEY_PAGE_DOWN, keyboard::scan_code::page_down);
    bind(GLFW_KEY_HOME, keyboard::scan_code::home);
    bind(GLFW_KEY_END, keyboard::scan_code::end);
    bind(GLFW_KEY_CAPS_LOCK, keyboard::scan_code::caps_lock);
    bind(GLFW_KEY_SCROLL_LOCK, keyboard::scan_code::scroll_lock);
    bind(GLFW_KEY_NUM_LOCK, keyboard::scan_code::num_lock);
    bind(GLFW_KEY_PRINT_SCREEN, keyboard::scan_code::print_screen);
    bind(GLFW_KEY_PAUSE, keyboard::scan_code::pause);
    bind(GLFW_KEY_F1, keyboard::scan_code::F1);
    bind(GLFW_KEY_F2, keyboard::scan_code::F2);
    bind(GLFW_KEY_F3, keyboard::scan_code::F3);
    bind(GLFW_KEY_F4, keyboard::scan_code::F4);
    bind(GLFW_KEY_F5, keyboard::scan_code::F5);
    bind(GLFW_KEY_F6, keyboard::scan_code::F6);
    bind(GLFW_KEY_F7, keyboard::scan_code::F7);
    bind(GLFW_KEY_F8, keyboard::scan_code::F8);
    bind(GLFW_KEY_F9, keyboard::scan_code::F9);
    bind(GLFW_KEY_F10, keyboard::scan_code::F10);
    bind(GLFW_KEY_F11, keyboard::scan_code::F11);
    bind(GLFW_KEY_F12, keyboard::scan_code::F12);
    bind(GLFW_KEY_F13, keyboard::scan_code::F13);
    bind(GLFW_KEY_F14, keyboard::scan_code::F14);
    bind(GLFW_KEY_F15, keyboard::scan_code::F15);
    bind(GLFW_KEY_F16, keyboard::scan_code::F16);
    bind(GLFW_KEY_F17, keyboard::scan_code::F17);
    bind(GLFW_KEY_F18, keyboard::scan_code::F18);
    bind(GLFW_KEY_F19, keyboard::scan_code::F19);
    bind(GLFW_KEY_F20, keyboard::scan_code::F20);
    bind(GLFW_KEY_F21, keyboard::scan_code::F21);
    bind(GLFW_KEY_F22, keyboard::scan_code::F22);
    bind(GLFW_KEY_F23, keyboard::scan_code::F23);
    bind(GLFW_KEY_F24, keyboard::scan_code::F24);
    bind(GLFW_KEY_F25, keyboard::scan_code::execute);
    bind(GLFW_KEY_KP_0, keyboard::scan_code::kp_0);
    bind(GLFW_KEY_KP_1, keyboard::scan_code::kp_1);
    bind(GLFW_KEY_KP_2, keyboard::scan_code::kp_2);
    bind(GLFW_KEY_KP_3, keyboard::scan_code::kp_3);
    bind(GLFW_KEY_KP_4, keyboard::scan_code::kp_4);
    bind(GLFW_KEY_KP_5, keyboard::scan_code::kp_5);
    bind(GLFW_KEY_KP_6, keyboard::scan_code::kp_6);
    bind(GLFW_KEY_KP_7, keyboard::scan_code::kp_7);
    bind(GLFW_KEY_KP_8, keyboard::scan_code::kp_8);
    bind(GLFW_KEY_KP_9, keyboard::scan_code::kp_9);
    bind(GLFW_KEY_KP_DECIMAL, keyboard::scan_code::kp_decimal);
    bind(GLFW_KEY_KP_DIVIDE, keyboard::scan_code::kp_divide);
    bind(GLFW_KEY_KP_MULTIPLY, keyboard::scan_code::kp_multiply);
    bind(GLFW_KEY_KP_SUBTRACT, keyboard::scan_code::kp_subtract);
    bind(GLFW_KEY_KP_ADD, keyboard::scan_code::kp_add);
    bind(GLFW_KEY_KP_ENTER, keyboard::scan_code::kp_enter);
    bind(GLFW_KEY_KP_EQUAL, keyboard::scan_code::kp_equal);
    bind(GLFW_KEY_LEFT_SHIFT, keyboard::scan_code::left_shift);
    bind(GLFW_KEY_LEFT_CONTROL, keyboard::scan_code::left_control);
    bind(GLFW_KEY_LEFT_ALT, keyboard::scan_code::left_alt);
    bind(GLFW_KEY_LEFT_SUPER, keyboard::scan_code::left_super);
    bind(GLFW_KEY_RIGHT_SHIFT, keyboard::scan_code::right_shift);
    bind(GLFW_KEY_RIGHT_CONTROL, keyboard::scan_code::right_control);
    bind(GLFW_KEY_RIGHT_ALT, keyboard::scan_code::right_alt);
    bind(GLFW_KEY_RIGHT_SUPER, keyboard::scan_code::right_super);
    bind(GLFW_KEY_MENU, keyboard::scan_code::menu);
  };

  static constexpr auto load_logical_keys = [] -> void {
    logical_keys_[GLFW_KEY_SPACE] = keyboard::key::spacebar;
    logical_keys_[GLFW_KEY_APOSTROPHE] = keyboard::key::apostrophe;
    logical_keys_[GLFW_KEY_COMMA] = keyboard::key::comma;
    logical_keys_[GLFW_KEY_MINUS] = keyboard::key::minus;
    logical_keys_[GLFW_KEY_PERIOD] = keyboard::key::period;
    logical_keys_[GLFW_KEY_SLASH] = keyboard::key::slash;
    logical_keys_[GLFW_KEY_0] = keyboard::key::_0;
    logical_keys_[GLFW_KEY_1] = keyboard::key::_1;
    logical_keys_[GLFW_KEY_2] = keyboard::key::_2;
    logical_keys_[GLFW_KEY_3] = keyboard::key::_3;
    logical_keys_[GLFW_KEY_4] = keyboard::key::_4;
    logical_keys_[GLFW_KEY_5] = keyboard::key::_5;
    logical_keys_[GLFW_KEY_6] = keyboard::key::_6;
    logical_keys_[GLFW_KEY_7] = keyboard::key::_7;
    logical_keys_[GLFW_KEY_8] = keyboard::key::_8;
    logical_keys_[GLFW_KEY_9] = keyboard::key::_9;
    logical_keys_[GLFW_KEY_SEMICOLON] = keyboard::key::semicolon;
    logical_keys_[GLFW_KEY_EQUAL] = keyboard::key::equal;
    logical_keys_[GLFW_KEY_A] = keyboard::key::a;
    logical_keys_[GLFW_KEY_B] = keyboard::key::b;
    logical_keys_[GLFW_KEY_C] = keyboard::key::c;
    logical_keys_[GLFW_KEY_D] = keyboard::key::d;
    logical_keys_[GLFW_KEY_E] = keyboard::key::e;
    logical_keys_[GLFW_KEY_F] = keyboard::key::f;
    logical_keys_[GLFW_KEY_G] = keyboard::key::g;
    logical_keys_[GLFW_KEY_H] = keyboard::key::h;
    logical_keys_[GLFW_KEY_I] = keyboard::key::i;
    logical_keys_[GLFW_KEY_J] = keyboard::key::j;
    logical_keys_[GLFW_KEY_K] = keyboard::key::k;
    logical_keys_[GLFW_KEY_L] = keyboard::key::l;
    logical_keys_[GLFW_KEY_M] = keyboard::key::m;
    logical_keys_[GLFW_KEY_N] = keyboard::key::n;
    logical_keys_[GLFW_KEY_O] = keyboard::key::o;
    logical_keys_[GLFW_KEY_P] = keyboard::key::p;
    logical_keys_[GLFW_KEY_Q] = keyboard::key::q;
    logical_keys_[GLFW_KEY_R] = keyboard::key::r;
    logical_keys_[GLFW_KEY_S] = keyboard::key::s;
    logical_keys_[GLFW_KEY_T] = keyboard::key::t;
    logical_keys_[GLFW_KEY_U] = keyboard::key::u;
    logical_keys_[GLFW_KEY_V] = keyboard::key::v;
    logical_keys_[GLFW_KEY_W] = keyboard::key::w;
    logical_keys_[GLFW_KEY_X] = keyboard::key::x;
    logical_keys_[GLFW_KEY_Y] = keyboard::key::y;
    logical_keys_[GLFW_KEY_Z] = keyboard::key::z;
    logical_keys_[GLFW_KEY_LEFT_BRACKET] = keyboard::key::left_bracket;
    logical_keys_[GLFW_KEY_BACKSLASH] = keyboard::key::backslash;
    logical_keys_[GLFW_KEY_RIGHT_BRACKET] = keyboard::key::right_bracket;
    logical_keys_[GLFW_KEY_GRAVE_ACCENT] = keyboard::key::grave_accent;
    logical_keys_[GLFW_KEY_WORLD_1] = keyboard::key::world_1;
    logical_keys_[GLFW_KEY_WORLD_2] = keyboard::key::world_2;
    logical_keys_[GLFW_KEY_ESCAPE] = keyboard::key::escape;
    logical_keys_[GLFW_KEY_ENTER] = keyboard::key::enter;
    logical_keys_[GLFW_KEY_TAB] = keyboard::key::tab;
    logical_keys_[GLFW_KEY_BACKSPACE] = keyboard::key::backspace;
    logical_keys_[GLFW_KEY_INSERT] = keyboard::key::insert;
    logical_keys_[GLFW_KEY_DELETE] = keyboard::key::delete_key;
    logical_keys_[GLFW_KEY_RIGHT] = keyboard::key::arrow_right;
    logical_keys_[GLFW_KEY_LEFT] = keyboard::key::arrow_left;
    logical_keys_[GLFW_KEY_DOWN] = keyboard::key::arrow_down;
    logical_keys_[GLFW_KEY_UP] = keyboard::key::arrow_up;
    logical_keys_[GLFW_KEY_PAGE_UP] = keyboard::key::page_up;
    logical_keys_[GLFW_KEY_PAGE_DOWN] = keyboard::key::page_down;
    logical_keys_[GLFW_KEY_HOME] = keyboard::key::home;
    logical_keys_[GLFW_KEY_END] = keyboard::key::end;
    logical_keys_[GLFW_KEY_CAPS_LOCK] = keyboard::key::caps_lock;
    logical_keys_[GLFW_KEY_SCROLL_LOCK] = keyboard::key::scroll_lock;
    logical_keys_[GLFW_KEY_PRINT_SCREEN] = keyboard::key::print_screen;
    logical_keys_[GLFW_KEY_NUM_LOCK] = keyboard::key::num_lock;
    logical_keys_[GLFW_KEY_PAUSE] = keyboard::key::pause;
    logical_keys_[GLFW_KEY_F1] = keyboard::key::F1;
    logical_keys_[GLFW_KEY_F2] = keyboard::key::F2;
    logical_keys_[GLFW_KEY_F3] = keyboard::key::F3;
    logical_keys_[GLFW_KEY_F4] = keyboard::key::F4;
    logical_keys_[GLFW_KEY_F5] = keyboard::key::F5;
    logical_keys_[GLFW_KEY_F6] = keyboard::key::F6;
    logical_keys_[GLFW_KEY_F7] = keyboard::key::F7;
    logical_keys_[GLFW_KEY_F8] = keyboard::key::F8;
    logical_keys_[GLFW_KEY_F9] = keyboard::key::F9;
    logical_keys_[GLFW_KEY_F10] = keyboard::key::F10;
    logical_keys_[GLFW_KEY_F11] = keyboard::key::F11;
    logical_keys_[GLFW_KEY_F12] = keyboard::key::F12;
    logical_keys_[GLFW_KEY_F13] = keyboard::key::F13;
    logical_keys_[GLFW_KEY_F14] = keyboard::key::F14;
    logical_keys_[GLFW_KEY_F15] = keyboard::key::F15;
    logical_keys_[GLFW_KEY_F16] = keyboard::key::F16;
    logical_keys_[GLFW_KEY_F17] = keyboard::key::F17;
    logical_keys_[GLFW_KEY_F18] = keyboard::key::F18;
    logical_keys_[GLFW_KEY_F19] = keyboard::key::F19;
    logical_keys_[GLFW_KEY_F20] = keyboard::key::F20;
    logical_keys_[GLFW_KEY_F21] = keyboard::key::F21;
    logical_keys_[GLFW_KEY_F22] = keyboard::key::F22;
    logical_keys_[GLFW_KEY_F23] = keyboard::key::F23;
    logical_keys_[GLFW_KEY_F24] = keyboard::key::F24;
    logical_keys_[GLFW_KEY_F25] = keyboard::key::execute;
    logical_keys_[GLFW_KEY_KP_0] = keyboard::key::kp_0;
    logical_keys_[GLFW_KEY_KP_1] = keyboard::key::kp_1;
    logical_keys_[GLFW_KEY_KP_2] = keyboard::key::kp_2;
    logical_keys_[GLFW_KEY_KP_3] = keyboard::key::kp_3;
    logical_keys_[GLFW_KEY_KP_4] = keyboard::key::kp_4;
    logical_keys_[GLFW_KEY_KP_5] = keyboard::key::kp_5;
    logical_keys_[GLFW_KEY_KP_6] = keyboard::key::kp_6;
    logical_keys_[GLFW_KEY_KP_7] = keyboard::key::kp_7;
    logical_keys_[GLFW_KEY_KP_8] = keyboard::key::kp_8;
    logical_keys_[GLFW_KEY_KP_9] = keyboard::key::kp_9;
    logical_keys_[GLFW_KEY_KP_DECIMAL] = keyboard::key::kp_decimal;
    logical_keys_[GLFW_KEY_KP_DIVIDE] = keyboard::key::kp_divide;
    logical_keys_[GLFW_KEY_KP_MULTIPLY] = keyboard::key::kp_multiply;
    logical_keys_[GLFW_KEY_KP_SUBTRACT] = keyboard::key::kp_subtract;
    logical_keys_[GLFW_KEY_KP_ADD] = keyboard::key::kp_add;
    logical_keys_[GLFW_KEY_KP_ENTER] = keyboard::key::kp_enter;
    logical_keys_[GLFW_KEY_KP_EQUAL] = keyboard::key::kp_equal;
    logical_keys_[GLFW_KEY_LEFT_SHIFT] = keyboard::key::left_shift;
    logical_keys_[GLFW_KEY_LEFT_CONTROL] = keyboard::key::left_control;
    logical_keys_[GLFW_KEY_LEFT_ALT] = keyboard::key::left_alt;
    logical_keys_[GLFW_KEY_LEFT_SUPER] = keyboard::key::left_super;
    logical_keys_[GLFW_KEY_RIGHT_SHIFT] = keyboard::key::right_shift;
    logical_keys_[GLFW_KEY_RIGHT_CONTROL] = keyboard::key::right_control;
    logical_keys_[GLFW_KEY_RIGHT_ALT] = keyboard::key::right_alt;
    logical_keys_[GLFW_KEY_RIGHT_SUPER] = keyboard::key::right_super;
    logical_keys_[GLFW_KEY_MENU] = keyboard::key::menu;
  };

public:
  glfw(typename TPlatform::window_handler_pointer_type window,
       const TPlatform &platform) {
    load_logical_keys();
    load_physical_scancodes(platform);
    platform.set_key_callback(window, key_callback);
  }
};

template <class TPlatform>
std::array<keyboard::key, GLFW_KEY_LAST + 1> glfw<TPlatform>::logical_keys_ =
    {};
template <class TPlatform>
std::vector<keyboard::scan_code> glfw<TPlatform>::physical_keys_ = {};
} // namespace jage::input::adapters
