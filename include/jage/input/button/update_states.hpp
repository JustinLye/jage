#pragma once

#include <jage/input/button/status.hpp>

#include <array>

namespace jage::input::button {
static constexpr auto update_states =
    []<class TDriver, class TState>(const TDriver &driver,
                                    TState &state) -> void {
  auto &buttons = state.buttons;

  for (auto index = 0UZ; index < std::size(buttons); ++index) {
    if (buttons[index]) {
      if (const auto button = static_cast<typename TState::button_type>(index);
          driver.is_down(button)) {
        state.button_states[button].status = button::status::down;
      }
    }
  }
};
} // namespace jage::input::button