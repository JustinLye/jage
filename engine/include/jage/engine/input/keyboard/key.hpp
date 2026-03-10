#pragma once

#include <cstdint>
#include <string_view>

namespace jage::input::keyboard {
enum class key : std::uint8_t {
  unidentified,
  spacebar,
  a,
  b,
  c,
  d,
  e,
  f,
  g,
  h,
  i,
  j,
  k,
  l,
  m,
  n,
  o,
  p,
  q,
  r,
  s,
  t,
  u,
  v,
  w,
  x,
  y,
  z,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  F13,
  F14,
  F15,
  F16,
  F17,
  F18,
  F19,
  F20,
  F21,
  F22,
  F23,
  F24,
  execute,
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
  apostrophe,
  comma,
  minus,
  period,
  slash,
  semicolon,
  equal,
  left_bracket,
  backslash,
  right_bracket,
  grave_accent,
  arrow_up,
  arrow_down,
  arrow_left,
  arrow_right,
  insert,
  delete_key,
  home,
  end,
  page_up,
  page_down,
  enter,
  tab,
  backspace,
  caps_lock,
  left_shift,
  right_shift,
  left_control,
  right_control,
  left_alt,
  right_alt,
  left_super,
  right_super,
  menu,
  print_screen,
  scroll_lock,
  num_lock,
  pause,
  kp_0,
  kp_1,
  kp_2,
  kp_3,
  kp_4,
  kp_5,
  kp_6,
  kp_7,
  kp_8,
  kp_9,
  kp_decimal,
  kp_divide,
  kp_multiply,
  kp_subtract,
  kp_add,
  kp_enter,
  kp_equal,
  world_1,
  world_2,
  escape,
  BEGIN = unidentified,
  END = escape,
};

[[nodiscard]] constexpr auto
serialize(const key input_key) -> std::string_view {
  switch (input_key) {
  case key::unidentified:
    return "unidentified";
  case key::spacebar:
    return "spacebar";
  case key::a:
    return "a";
  case key::b:
    return "b";
  case key::c:
    return "c";
  case key::d:
    return "d";
  case key::e:
    return "e";
  case key::f:
    return "f";
  case key::g:
    return "g";
  case key::h:
    return "h";
  case key::i:
    return "i";
  case key::j:
    return "j";
  case key::k:
    return "k";
  case key::l:
    return "l";
  case key::m:
    return "m";
  case key::n:
    return "n";
  case key::o:
    return "o";
  case key::p:
    return "p";
  case key::q:
    return "q";
  case key::r:
    return "r";
  case key::s:
    return "s";
  case key::t:
    return "t";
  case key::u:
    return "u";
  case key::v:
    return "v";
  case key::w:
    return "w";
  case key::x:
    return "x";
  case key::y:
    return "y";
  case key::z:
    return "z";
  case key::F1:
    return "F1";
  case key::F2:
    return "F2";
  case key::F3:
    return "F3";
  case key::F4:
    return "F4";
  case key::F5:
    return "F5";
  case key::F6:
    return "F6";
  case key::F7:
    return "F7";
  case key::F8:
    return "F8";
  case key::F9:
    return "F9";
  case key::F10:
    return "F10";
  case key::F11:
    return "F11";
  case key::F12:
    return "F12";
  case key::F13:
    return "F13";
  case key::F14:
    return "F14";
  case key::F15:
    return "F15";
  case key::F16:
    return "F16";
  case key::F17:
    return "F17";
  case key::F18:
    return "F18";
  case key::F19:
    return "F19";
  case key::F20:
    return "F20";
  case key::F21:
    return "F21";
  case key::F22:
    return "F22";
  case key::F23:
    return "F23";
  case key::F24:
    return "F24";
  case key::execute:
    return "execute";
  case key::_0:
    return "0";
  case key::_1:
    return "1";
  case key::_2:
    return "2";
  case key::_3:
    return "3";
  case key::_4:
    return "4";
  case key::_5:
    return "5";
  case key::_6:
    return "6";
  case key::_7:
    return "7";
  case key::_8:
    return "8";
  case key::_9:
    return "9";
  case key::apostrophe:
    return "apostrophe";
  case key::comma:
    return "comma";
  case key::minus:
    return "minus";
  case key::period:
    return "period";
  case key::slash:
    return "slash";
  case key::semicolon:
    return "semicolon";
  case key::equal:
    return "equal";
  case key::left_bracket:
    return "left_bracket";
  case key::backslash:
    return "backslash";
  case key::right_bracket:
    return "right_bracket";
  case key::grave_accent:
    return "grave_accent";
  case key::arrow_up:
    return "arrow_up";
  case key::arrow_down:
    return "arrow_down";
  case key::arrow_left:
    return "arrow_left";
  case key::arrow_right:
    return "arrow_right";
  case key::insert:
    return "insert";
  case key::delete_key:
    return "delete_key";
  case key::home:
    return "home";
  case key::end:
    return "end";
  case key::page_up:
    return "page_up";
  case key::page_down:
    return "page_down";
  case key::enter:
    return "enter";
  case key::tab:
    return "tab";
  case key::backspace:
    return "backspace";
  case key::caps_lock:
    return "caps_lock";
  case key::left_shift:
    return "left_shift";
  case key::right_shift:
    return "right_shift";
  case key::left_control:
    return "left_control";
  case key::right_control:
    return "right_control";
  case key::left_alt:
    return "left_alt";
  case key::right_alt:
    return "right_alt";
  case key::left_super:
    return "left_super";
  case key::right_super:
    return "right_super";
  case key::menu:
    return "menu";
  case key::print_screen:
    return "print_screen";
  case key::scroll_lock:
    return "scroll_lock";
  case key::num_lock:
    return "num_lock";
  case key::pause:
    return "pause";
  case key::kp_0:
    return "kp_0";
  case key::kp_1:
    return "kp_1";
  case key::kp_2:
    return "kp_2";
  case key::kp_3:
    return "kp_3";
  case key::kp_4:
    return "kp_4";
  case key::kp_5:
    return "kp_5";
  case key::kp_6:
    return "kp_6";
  case key::kp_7:
    return "kp_7";
  case key::kp_8:
    return "kp_8";
  case key::kp_9:
    return "kp_9";
  case key::kp_decimal:
    return "kp_decimal";
  case key::kp_divide:
    return "kp_divide";
  case key::kp_multiply:
    return "kp_multiply";
  case key::kp_subtract:
    return "kp_subtract";
  case key::kp_add:
    return "kp_add";
  case key::kp_enter:
    return "kp_enter";
  case key::kp_equal:
    return "kp_equal";
  case key::world_1:
    return "world_1";
  case key::world_2:
    return "world_2";
  case key::escape:
    return "escape";
  default:
    return "unknown enumerator";
  }
}

} // namespace jage::input::keyboard
