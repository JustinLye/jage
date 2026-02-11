#pragma once

#include <cstdint>

namespace jage::input::keyboard {
enum class scan_code : std::uint16_t {
  unidentified = 0,
  a = 0x04,
  escape = 0x29,
};
}