#pragma once

#include <cstddef>
#include <utility>

namespace jage::input::button {
static constexpr auto monitor_button = [](auto &state,
                                          const auto button) -> void {
  state.buttons[static_cast<std::size_t>(std::to_underlying(button))] = true;
};
} // namespace jage::input::button