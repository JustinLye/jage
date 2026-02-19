#pragma once

#include <cstdint>
#include <utility>

namespace jage::input {
enum class modifier : std::uint8_t {
  left_control,
  left_shift,
  left_alt,
  left_gui,
  right_control,
  right_shift,
  right_alt,
  right_gui,
  caps_lock,
  num_lock,
  last = num_lock,
};

static constexpr std::uint8_t modifier_count =
    std::to_underlying(modifier::last) + 1;
} // namespace jage::input
