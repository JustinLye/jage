#pragma once

#include <cstdint>

namespace jage::input::button {
enum class status : std::uint8_t {
  up,
  down,
};
}