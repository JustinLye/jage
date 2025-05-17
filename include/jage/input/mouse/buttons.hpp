#pragma once

#include <cstdint>

namespace jage::input::mouse {
enum class buttons : std::uint8_t {
  left_click,
  middle_click,
  right_click,
  BEGIN = left_click,
  END = right_click,
};
}