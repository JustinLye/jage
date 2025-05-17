#pragma once

#include <cstdint>

namespace jage::input::keyboard {
enum class keys : std::uint8_t {
  spacebar,
  a,
  escape,
  BEGIN = spacebar,
  END = escape,
};

}