#pragma once

#include <cstdint>

namespace jage::input::keyboard {
enum class action : std::uint8_t {
  press,
  release,
  repeat,
};
}