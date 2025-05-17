#pragma once

#include <cstdint>

namespace jage {
enum class scheduled_action_status : std::uint8_t {
  paused,
  active,
  canceled,
  complete,
};
}