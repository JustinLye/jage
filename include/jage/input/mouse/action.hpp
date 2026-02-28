#pragma once

#include <cstdint>
#include <string_view>

namespace jage::input::mouse {
enum class action : std::uint8_t {
  release,
  press,
};

[[nodiscard]] constexpr auto serialize(const action input_action)
    -> std::string_view {
  switch (input_action) {
  case action::release:
    return "release";
  case action::press:
    return "press";
  default:
    return "unknown enumerator";
  }
}
} // namespace jage::input::mouse