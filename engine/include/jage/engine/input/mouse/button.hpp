#pragma once

#include <cstdint>
#include <string_view>
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

[[nodiscard]] constexpr auto serialize(const button input_button)
    -> std::string_view {
  switch (input_button) {
  case button::left:
    return "left";
  case button::right:
    return "right";
  case button::middle:
    return "middle";
  case button::back:
    return "back";
  case button::forward:
    return "forward";
  case button::gesture:
    return "gesture";
  case button::action:
    return "action";
  case button::unidentified:
    return "unidentified";
  default:
    return "unknown enumerator";
  }
}
} // namespace jage::input::mouse