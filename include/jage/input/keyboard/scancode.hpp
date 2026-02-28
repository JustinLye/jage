#pragma once

#include <cstdint>
#include <string_view>

namespace jage::input::keyboard {
enum class scancode : std::uint16_t {
  unidentified = 0,
  a = 0x04,
  b = 0x05,
  c = 0x06,
  d = 0x07,
  e = 0x08,
  f = 0x09,
  g = 0x0A,
  h = 0x0B,
  i = 0x0C,
  j = 0x0D,
  k = 0x0E,
  l = 0x0F,
  m = 0x10,
  n = 0x11,
  o = 0x12,
  p = 0x13,
  q = 0x14,
  r = 0x15,
  s = 0x16,
  t = 0x17,
  u = 0x18,
  v = 0x19,
  w = 0x1A,
  x = 0x1B,
  y = 0x1C,
  z = 0x1D,
  _1 = 0x1E,
  _2 = 0x1F,
  _3 = 0x20,
  _4 = 0x21,
  _5 = 0x22,
  _6 = 0x23,
  _7 = 0x24,
  _8 = 0x25,
  _9 = 0x26,
  _0 = 0x27,
  enter = 0x28,
  escape = 0x29,
  backspace = 0x2A,
  tab = 0x2B,
  spacebar = 0x2C,
  minus = 0x2D,
  equal = 0x2E,
  left_bracket = 0x2F,
  right_bracket = 0x30,
  backslash = 0x31,
  semicolon = 0x33,
  apostrophe = 0x34,
  grave_accent = 0x35,
  comma = 0x36,
  period = 0x37,
  slash = 0x38,
  caps_lock = 0x39,
  F1 = 0x3A,
  F2 = 0x3B,
  F3 = 0x3C,
  F4 = 0x3D,
  F5 = 0x3E,
  F6 = 0x3F,
  F7 = 0x40,
  F8 = 0x41,
  F9 = 0x42,
  F10 = 0x43,
  F11 = 0x44,
  F12 = 0x45,
  print_screen = 0x46,
  scroll_lock = 0x47,
  pause = 0x48,
  insert = 0x49,
  home = 0x4A,
  page_up = 0x4B,
  delete_key = 0x4C,
  end = 0x4D,
  page_down = 0x4E,
  arrow_right = 0x4F,
  arrow_left = 0x50,
  arrow_down = 0x51,
  arrow_up = 0x52,
  num_lock = 0x53,
  kp_divide = 0x54,
  kp_multiply = 0x55,
  kp_subtract = 0x56,
  kp_add = 0x57,
  kp_enter = 0x58,
  kp_1 = 0x59,
  kp_2 = 0x5A,
  kp_3 = 0x5B,
  kp_4 = 0x5C,
  kp_5 = 0x5D,
  kp_6 = 0x5E,
  kp_7 = 0x5F,
  kp_8 = 0x60,
  kp_9 = 0x61,
  kp_0 = 0x62,
  kp_decimal = 0x63,
  world_1 = 0x87,
  world_2 = 0x88,
  kp_equal = 0x67,
  menu = 0x65,
  F13 = 0x68,
  F14 = 0x69,
  F15 = 0x6A,
  F16 = 0x6B,
  F17 = 0x6C,
  F18 = 0x6D,
  F19 = 0x6E,
  F20 = 0x6F,
  F21 = 0x70,
  F22 = 0x71,
  F23 = 0x72,
  F24 = 0x73,
  execute = 0x74,
  left_control = 0xE0,
  left_shift = 0xE1,
  left_alt = 0xE2,
  left_super = 0xE3,
  right_control = 0xE4,
  right_shift = 0xE5,
  right_alt = 0xE6,
  right_super = 0xE7,
};

[[nodiscard]] constexpr auto
serialize(const scancode input_scancode) -> std::string_view {
  switch (input_scancode) {
  case scancode::unidentified:
    return "unidentified";
  case scancode::a:
    return "a";
  case scancode::b:
    return "b";
  case scancode::c:
    return "c";
  case scancode::d:
    return "d";
  case scancode::e:
    return "e";
  case scancode::f:
    return "f";
  case scancode::g:
    return "g";
  case scancode::h:
    return "h";
  case scancode::i:
    return "i";
  case scancode::j:
    return "j";
  case scancode::k:
    return "k";
  case scancode::l:
    return "l";
  case scancode::m:
    return "m";
  case scancode::n:
    return "n";
  case scancode::o:
    return "o";
  case scancode::p:
    return "p";
  case scancode::q:
    return "q";
  case scancode::r:
    return "r";
  case scancode::s:
    return "s";
  case scancode::t:
    return "t";
  case scancode::u:
    return "u";
  case scancode::v:
    return "v";
  case scancode::w:
    return "w";
  case scancode::x:
    return "x";
  case scancode::y:
    return "y";
  case scancode::z:
    return "z";
  case scancode::_1:
    return "1";
  case scancode::_2:
    return "2";
  case scancode::_3:
    return "3";
  case scancode::_4:
    return "4";
  case scancode::_5:
    return "5";
  case scancode::_6:
    return "6";
  case scancode::_7:
    return "7";
  case scancode::_8:
    return "8";
  case scancode::_9:
    return "9";
  case scancode::_0:
    return "0";
  case scancode::enter:
    return "enter";
  case scancode::escape:
    return "escape";
  case scancode::backspace:
    return "backspace";
  case scancode::tab:
    return "tab";
  case scancode::spacebar:
    return "spacebar";
  case scancode::minus:
    return "minus";
  case scancode::equal:
    return "equal";
  case scancode::left_bracket:
    return "left_bracket";
  case scancode::right_bracket:
    return "right_bracket";
  case scancode::backslash:
    return "backslash";
  case scancode::semicolon:
    return "semicolon";
  case scancode::apostrophe:
    return "apostrophe";
  case scancode::grave_accent:
    return "grave_accent";
  case scancode::comma:
    return "comma";
  case scancode::period:
    return "period";
  case scancode::slash:
    return "slash";
  case scancode::caps_lock:
    return "caps_lock";
  case scancode::F1:
    return "F1";
  case scancode::F2:
    return "F2";
  case scancode::F3:
    return "F3";
  case scancode::F4:
    return "F4";
  case scancode::F5:
    return "F5";
  case scancode::F6:
    return "F6";
  case scancode::F7:
    return "F7";
  case scancode::F8:
    return "F8";
  case scancode::F9:
    return "F9";
  case scancode::F10:
    return "F10";
  case scancode::F11:
    return "F11";
  case scancode::F12:
    return "F12";
  case scancode::print_screen:
    return "print_screen";
  case scancode::scroll_lock:
    return "scroll_lock";
  case scancode::pause:
    return "pause";
  case scancode::insert:
    return "insert";
  case scancode::home:
    return "home";
  case scancode::page_up:
    return "page_up";
  case scancode::delete_key:
    return "delete_key";
  case scancode::end:
    return "end";
  case scancode::page_down:
    return "page_down";
  case scancode::arrow_right:
    return "arrow_right";
  case scancode::arrow_left:
    return "arrow_left";
  case scancode::arrow_down:
    return "arrow_down";
  case scancode::arrow_up:
    return "arrow_up";
  case scancode::num_lock:
    return "num_lock";
  case scancode::kp_divide:
    return "kp_divide";
  case scancode::kp_multiply:
    return "kp_multiply";
  case scancode::kp_subtract:
    return "kp_subtract";
  case scancode::kp_add:
    return "kp_add";
  case scancode::kp_enter:
    return "kp_enter";
  case scancode::kp_1:
    return "kp_1";
  case scancode::kp_2:
    return "kp_2";
  case scancode::kp_3:
    return "kp_3";
  case scancode::kp_4:
    return "kp_4";
  case scancode::kp_5:
    return "kp_5";
  case scancode::kp_6:
    return "kp_6";
  case scancode::kp_7:
    return "kp_7";
  case scancode::kp_8:
    return "kp_8";
  case scancode::kp_9:
    return "kp_9";
  case scancode::kp_0:
    return "kp_0";
  case scancode::kp_decimal:
    return "kp_decimal";
  case scancode::world_1:
    return "world_1";
  case scancode::world_2:
    return "world_2";
  case scancode::kp_equal:
    return "kp_equal";
  case scancode::menu:
    return "menu";
  case scancode::F13:
    return "F13";
  case scancode::F14:
    return "F14";
  case scancode::F15:
    return "F15";
  case scancode::F16:
    return "F16";
  case scancode::F17:
    return "F17";
  case scancode::F18:
    return "F18";
  case scancode::F19:
    return "F19";
  case scancode::F20:
    return "F20";
  case scancode::F21:
    return "F21";
  case scancode::F22:
    return "F22";
  case scancode::F23:
    return "F23";
  case scancode::F24:
    return "F24";
  case scancode::execute:
    return "execute";
  case scancode::left_control:
    return "left_control";
  case scancode::left_shift:
    return "left_shift";
  case scancode::left_alt:
    return "left_alt";
  case scancode::left_super:
    return "left_super";
  case scancode::right_control:
    return "right_control";
  case scancode::right_shift:
    return "right_shift";
  case scancode::right_alt:
    return "right_alt";
  case scancode::right_super:
    return "right_super";
  default:
    return "unknown enumerator";
  }
}

} // namespace jage::input::keyboard
