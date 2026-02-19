#pragma once

#include <cstdint>

namespace jage::input::mouse {
enum class action : std::uint8_t {
  release,
  press,
};
}