#pragma once

#include <cstdint>

namespace jage::time {
enum class cache_match_status : std::uint8_t {
  matched,
  ahead,
  evicted,
};
}