#pragma once

#include <cstdint>

namespace jage::input::keyboard {
enum class key : std::uint8_t {
  spacebar,
  a,
  escape,
  BEGIN = spacebar,
  END = escape,
};

}
