#pragma once

#include <cstdint>

namespace jage::input::keyboard {
enum class modifier : std::uint8_t {
  left_control,
  left_shift,
  left_alt,
  left_gui,
  right_control,
  right_shift,
  right_alt,
  right_gui,
};

static constexpr std::uint8_t modifier_count = 8U;
} // namespace jage::input::keyboard