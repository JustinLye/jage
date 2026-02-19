#pragma once

#include <cstdint>
#include <utility>

namespace jage::input::mouse {
enum class button : std::uint8_t {
  left,
  right,
  middle,
  back,
  forward,
  gesture,
  action,
  unidentified,
  last = unidentified,
};

static constexpr auto button_count = std::to_underlying(button::last) + 1UZ;
} // namespace jage::input::mouse