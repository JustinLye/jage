#pragma once
#include <jage/external/glfw.hpp>
#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/events/key_press.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/scancode.hpp>
#include <jage/input/mouse/button.hpp>
#include <jage/input/mouse/events/click.hpp>
#include <jage/input/mouse/events/cursor/motion.hpp>
#include <jage/input/mouse/events/cursor/position.hpp>
#include <jage/input/mouse/events/horizontal_scroll.hpp>
#include <jage/input/mouse/events/vertical_scroll.hpp>
#include <jage/time/durations.hpp>

#include <array>
#include <utility>
#include <vector>

namespace jage::input::adapters {
// TODO: Make this a concept
template <class TPlatform> class glfw {

  static std::array<keyboard::key, GLFW_KEY_LAST + 1> logical_keys_;
  static std::vector<keyboard::scancode> physical_keys_;
  using duration_type = typename TPlatform::context_type::duration_type;
  using event_type = typename TPlatform::context_type::event_type;
  using window_handle_pointer_type =
      typename TPlatform::window_handle_pointer_type;

  static constexpr auto get_physical_key =
      [] [[nodiscard]] (const int scancode) -> keyboard::scancode {
    if (0 > scancode) [[unlikely]] {
      return keyboard::scancode::unidentified;
    } else if (static_cast<std::size_t>(scancode) >= std::size(physical_keys_))
        [[unlikely]] {
      return keyboard::scancode::unidentified;
    } else [[likely]] {
      return physical_keys_[static_cast<std::size_t>(scancode)];
    }
  };

  [[nodiscard]] static inline auto
  get_modifier(const auto mods) -> std::bitset<modifier_count> {

    auto modifier_bitset = std::bitset<modifier_count>{};

    const auto flip = [&](const auto... modifiers) -> void {
      (modifier_bitset.flip(std::to_underlying(modifiers)), ...);
    };

    if (mods != 0) [[unlikely]] {
      if (mods & GLFW_MOD_SHIFT) {
        flip(modifier::left_shift, modifier::right_shift);
      }
      if (mods & GLFW_MOD_ALT) {
        flip(modifier::left_alt, modifier::right_alt);
      }
      if (mods & GLFW_MOD_CONTROL) {
        flip(modifier::left_control, modifier::right_control);
      }
      if (mods & GLFW_MOD_SUPER) {
        flip(modifier::left_gui, modifier::right_gui);
      }
      if (mods & GLFW_MOD_CAPS_LOCK) {
        flip(modifier::caps_lock);
      }
      if (mods & GLFW_MOD_NUM_LOCK) {
        flip(modifier::num_lock);
      }
    }
    return modifier_bitset;
  }

  static constexpr auto get_current_timestamp =
      [] [[nodiscard]] -> duration_type {
    return time::cast<duration_type>(TPlatform::get_seconds_since_init());
  };

  static constexpr auto cursor_is_disabled =
      [] [[nodiscard]] (window_handle_pointer_type window) -> bool {
    return GLFW_CURSOR_DISABLED ==
           TPlatform::get_input_mode(window, GLFW_CURSOR);
  };

  static constexpr auto get_context(window_handle_pointer_type window) ->
      typename TPlatform::context_type & {
    return *static_cast<typename TPlatform::context_type *>(
        TPlatform::get_window_user_pointer(window));
  }

  static constexpr auto push_event(window_handle_pointer_type window,
                                   auto &&payload) -> void {
    auto &context = get_context(window);
    context.push(event_type{
        .timestamp = get_current_timestamp(),
        .payload = std::forward<decltype(payload)>(payload),
    });
  };

  static constexpr auto almost_equal = [] [[nodiscard]] (double lhs,
                                                         double rhs) -> bool {
    auto difference = std::abs(lhs - rhs);
    return difference <= std::numeric_limits<double>::epsilon() *
                             std::max(std::abs(lhs), std::abs(rhs));
  };

  static constexpr auto scroll_callback = [](window_handle_pointer_type window,
                                             double xoffset,
                                             double yoffset) -> void {
    if (0.0 != yoffset) {
      push_event(window, mouse::events::vertical_scroll{
                             .offset = yoffset,
                         });
    } else {
      push_event(window, mouse::events::horizontal_scroll{
                             .offset = xoffset,
                         });
    }
  };

  static constexpr auto cursor_position_callback =
      [](window_handle_pointer_type window, double xpos, double ypos) -> void {
    auto &[last_x_pos, last_y_pos] =
        get_context(window).last_known_cursor_position();
    if (almost_equal(xpos, last_x_pos) and almost_equal(ypos, last_y_pos)) {
      return;
    }
    if (cursor_is_disabled(window)) {
      push_event(window, mouse::events::cursor::motion{
                             .delta_x = xpos - last_x_pos,
                             .delta_y = ypos - last_y_pos,
                         });

    } else {
      push_event(window, mouse::events::cursor::position{
                             .x = xpos,
                             .y = ypos,
                         });
    }
    last_x_pos = xpos;
    last_y_pos = ypos;
  };

  static constexpr auto mouse_button_callback =
      [](window_handle_pointer_type window, int button, int action, int mods) {
        push_event(window, mouse::events::click{
                               .button = static_cast<mouse::button>(button),
                               .action = static_cast<mouse::action>(action),
                               .modifiers = get_modifier(mods),
                           });
      };

  static constexpr auto key_callback = [](window_handle_pointer_type window,
                                          int key, int scancode, int action,
                                          int mods) -> void {
    push_event(window,
               keyboard::events::key_press{
                   .key = key != GLFW_KEY_UNKNOWN ? logical_keys_[key]
                                                  : keyboard::key::unidentified,
                   .scancode = get_physical_key(scancode),
                   .action = static_cast<keyboard::action>(action),
                   .modifiers = get_modifier(mods),
               });
  };

  static constexpr auto load_physical_scancodes =
      [](const TPlatform &platform) -> void {
    auto bind = [&](const auto glfw_key,
                    const keyboard::scancode scancode) -> void {
      if (const auto os_scancode = platform.get_key_scancode(glfw_key);
          os_scancode != -1) {
        if (static_cast<std::size_t>(os_scancode) >=
            std::size(physical_keys_)) {
          physical_keys_.resize(static_cast<std::size_t>(os_scancode) + 1UZ,
                                keyboard::scancode::unidentified);
        }
        physical_keys_[os_scancode] = scancode;
      }
    };

    bind(GLFW_KEY_SPACE, keyboard::scancode::spacebar);
    bind(GLFW_KEY_APOSTROPHE, keyboard::scancode::apostrophe);
    bind(GLFW_KEY_COMMA, keyboard::scancode::comma);
    bind(GLFW_KEY_MINUS, keyboard::scancode::minus);
    bind(GLFW_KEY_PERIOD, keyboard::scancode::period);
    bind(GLFW_KEY_SLASH, keyboard::scancode::slash);
    bind(GLFW_KEY_0, keyboard::scancode::_0);
    bind(GLFW_KEY_1, keyboard::scancode::_1);
    bind(GLFW_KEY_2, keyboard::scancode::_2);
    bind(GLFW_KEY_3, keyboard::scancode::_3);
    bind(GLFW_KEY_4, keyboard::scancode::_4);
    bind(GLFW_KEY_5, keyboard::scancode::_5);
    bind(GLFW_KEY_6, keyboard::scancode::_6);
    bind(GLFW_KEY_7, keyboard::scancode::_7);
    bind(GLFW_KEY_8, keyboard::scancode::_8);
    bind(GLFW_KEY_9, keyboard::scancode::_9);
    bind(GLFW_KEY_SEMICOLON, keyboard::scancode::semicolon);
    bind(GLFW_KEY_EQUAL, keyboard::scancode::equal);
    bind(GLFW_KEY_A, keyboard::scancode::a);
    bind(GLFW_KEY_B, keyboard::scancode::b);
    bind(GLFW_KEY_C, keyboard::scancode::c);
    bind(GLFW_KEY_D, keyboard::scancode::d);
    bind(GLFW_KEY_E, keyboard::scancode::e);
    bind(GLFW_KEY_F, keyboard::scancode::f);
    bind(GLFW_KEY_G, keyboard::scancode::g);
    bind(GLFW_KEY_H, keyboard::scancode::h);
    bind(GLFW_KEY_I, keyboard::scancode::i);
    bind(GLFW_KEY_J, keyboard::scancode::j);
    bind(GLFW_KEY_K, keyboard::scancode::k);
    bind(GLFW_KEY_L, keyboard::scancode::l);
    bind(GLFW_KEY_M, keyboard::scancode::m);
    bind(GLFW_KEY_N, keyboard::scancode::n);
    bind(GLFW_KEY_O, keyboard::scancode::o);
    bind(GLFW_KEY_P, keyboard::scancode::p);
    bind(GLFW_KEY_Q, keyboard::scancode::q);
    bind(GLFW_KEY_R, keyboard::scancode::r);
    bind(GLFW_KEY_S, keyboard::scancode::s);
    bind(GLFW_KEY_T, keyboard::scancode::t);
    bind(GLFW_KEY_U, keyboard::scancode::u);
    bind(GLFW_KEY_V, keyboard::scancode::v);
    bind(GLFW_KEY_W, keyboard::scancode::w);
    bind(GLFW_KEY_X, keyboard::scancode::x);
    bind(GLFW_KEY_Y, keyboard::scancode::y);
    bind(GLFW_KEY_Z, keyboard::scancode::z);
    bind(GLFW_KEY_LEFT_BRACKET, keyboard::scancode::left_bracket);
    bind(GLFW_KEY_BACKSLASH, keyboard::scancode::backslash);
    bind(GLFW_KEY_RIGHT_BRACKET, keyboard::scancode::right_bracket);
    bind(GLFW_KEY_GRAVE_ACCENT, keyboard::scancode::grave_accent);
    bind(GLFW_KEY_WORLD_1, keyboard::scancode::world_1);
    bind(GLFW_KEY_WORLD_2, keyboard::scancode::world_2);
    bind(GLFW_KEY_ESCAPE, keyboard::scancode::escape);
    bind(GLFW_KEY_ENTER, keyboard::scancode::enter);
    bind(GLFW_KEY_TAB, keyboard::scancode::tab);
    bind(GLFW_KEY_BACKSPACE, keyboard::scancode::backspace);
    bind(GLFW_KEY_INSERT, keyboard::scancode::insert);
    bind(GLFW_KEY_DELETE, keyboard::scancode::delete_key);
    bind(GLFW_KEY_RIGHT, keyboard::scancode::arrow_right);
    bind(GLFW_KEY_LEFT, keyboard::scancode::arrow_left);
    bind(GLFW_KEY_DOWN, keyboard::scancode::arrow_down);
    bind(GLFW_KEY_UP, keyboard::scancode::arrow_up);
    bind(GLFW_KEY_PAGE_UP, keyboard::scancode::page_up);
    bind(GLFW_KEY_PAGE_DOWN, keyboard::scancode::page_down);
    bind(GLFW_KEY_HOME, keyboard::scancode::home);
    bind(GLFW_KEY_END, keyboard::scancode::end);
    bind(GLFW_KEY_CAPS_LOCK, keyboard::scancode::caps_lock);
    bind(GLFW_KEY_SCROLL_LOCK, keyboard::scancode::scroll_lock);
    bind(GLFW_KEY_NUM_LOCK, keyboard::scancode::num_lock);
    bind(GLFW_KEY_PRINT_SCREEN, keyboard::scancode::print_screen);
    bind(GLFW_KEY_PAUSE, keyboard::scancode::pause);
    bind(GLFW_KEY_F1, keyboard::scancode::F1);
    bind(GLFW_KEY_F2, keyboard::scancode::F2);
    bind(GLFW_KEY_F3, keyboard::scancode::F3);
    bind(GLFW_KEY_F4, keyboard::scancode::F4);
    bind(GLFW_KEY_F5, keyboard::scancode::F5);
    bind(GLFW_KEY_F6, keyboard::scancode::F6);
    bind(GLFW_KEY_F7, keyboard::scancode::F7);
    bind(GLFW_KEY_F8, keyboard::scancode::F8);
    bind(GLFW_KEY_F9, keyboard::scancode::F9);
    bind(GLFW_KEY_F10, keyboard::scancode::F10);
    bind(GLFW_KEY_F11, keyboard::scancode::F11);
    bind(GLFW_KEY_F12, keyboard::scancode::F12);
    bind(GLFW_KEY_F13, keyboard::scancode::F13);
    bind(GLFW_KEY_F14, keyboard::scancode::F14);
    bind(GLFW_KEY_F15, keyboard::scancode::F15);
    bind(GLFW_KEY_F16, keyboard::scancode::F16);
    bind(GLFW_KEY_F17, keyboard::scancode::F17);
    bind(GLFW_KEY_F18, keyboard::scancode::F18);
    bind(GLFW_KEY_F19, keyboard::scancode::F19);
    bind(GLFW_KEY_F20, keyboard::scancode::F20);
    bind(GLFW_KEY_F21, keyboard::scancode::F21);
    bind(GLFW_KEY_F22, keyboard::scancode::F22);
    bind(GLFW_KEY_F23, keyboard::scancode::F23);
    bind(GLFW_KEY_F24, keyboard::scancode::F24);
    bind(GLFW_KEY_F25, keyboard::scancode::execute);
    bind(GLFW_KEY_KP_0, keyboard::scancode::kp_0);
    bind(GLFW_KEY_KP_1, keyboard::scancode::kp_1);
    bind(GLFW_KEY_KP_2, keyboard::scancode::kp_2);
    bind(GLFW_KEY_KP_3, keyboard::scancode::kp_3);
    bind(GLFW_KEY_KP_4, keyboard::scancode::kp_4);
    bind(GLFW_KEY_KP_5, keyboard::scancode::kp_5);
    bind(GLFW_KEY_KP_6, keyboard::scancode::kp_6);
    bind(GLFW_KEY_KP_7, keyboard::scancode::kp_7);
    bind(GLFW_KEY_KP_8, keyboard::scancode::kp_8);
    bind(GLFW_KEY_KP_9, keyboard::scancode::kp_9);
    bind(GLFW_KEY_KP_DECIMAL, keyboard::scancode::kp_decimal);
    bind(GLFW_KEY_KP_DIVIDE, keyboard::scancode::kp_divide);
    bind(GLFW_KEY_KP_MULTIPLY, keyboard::scancode::kp_multiply);
    bind(GLFW_KEY_KP_SUBTRACT, keyboard::scancode::kp_subtract);
    bind(GLFW_KEY_KP_ADD, keyboard::scancode::kp_add);
    bind(GLFW_KEY_KP_ENTER, keyboard::scancode::kp_enter);
    bind(GLFW_KEY_KP_EQUAL, keyboard::scancode::kp_equal);
    bind(GLFW_KEY_LEFT_SHIFT, keyboard::scancode::left_shift);
    bind(GLFW_KEY_LEFT_CONTROL, keyboard::scancode::left_control);
    bind(GLFW_KEY_LEFT_ALT, keyboard::scancode::left_alt);
    bind(GLFW_KEY_LEFT_SUPER, keyboard::scancode::left_super);
    bind(GLFW_KEY_RIGHT_SHIFT, keyboard::scancode::right_shift);
    bind(GLFW_KEY_RIGHT_CONTROL, keyboard::scancode::right_control);
    bind(GLFW_KEY_RIGHT_ALT, keyboard::scancode::right_alt);
    bind(GLFW_KEY_RIGHT_SUPER, keyboard::scancode::right_super);
    bind(GLFW_KEY_MENU, keyboard::scancode::menu);
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
  static constexpr auto initialize = [](window_handle_pointer_type window,
                                        TPlatform &platform,
                                        const duration_type &epoch) {
    platform.set_seconds_since_init(time::cast<time::seconds>(epoch));
    load_logical_keys();
    load_physical_scancodes(platform);
    platform.set_key_callback(window, key_callback);
    platform.set_mouse_button_callback(window, mouse_button_callback);
    platform.set_cursor_position_callback(window, cursor_position_callback);
    platform.set_scroll_callback(window, scroll_callback);
  };
};

template <class TPlatform>
std::array<keyboard::key, GLFW_KEY_LAST + 1> glfw<TPlatform>::logical_keys_ =
    {};
template <class TPlatform>
std::vector<keyboard::scancode> glfw<TPlatform>::physical_keys_ = {};
} // namespace jage::input::adapters
