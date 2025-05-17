#pragma once

#include <cstdint>

namespace jage::input {
enum class status : std::uint8_t {
  up,
  down,
};
}