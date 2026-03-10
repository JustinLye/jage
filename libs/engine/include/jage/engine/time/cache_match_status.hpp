#pragma once

#include <cstdint>

namespace jage::engine::time {
enum class cache_match_status : std::uint8_t {
  matched,
  ahead,
  evicted,
};
}