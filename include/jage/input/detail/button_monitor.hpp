#pragma once

#include <jage/input/button/state.hpp>
#include <jage/input/button/states.hpp>
#include <jage/input/button/status.hpp>
#include <jage/input/detail/monitor.hpp>

#include <bitset>
#include <cstddef>
#include <utility>

namespace jage::input::detail {

template <class TButton> struct button_monitor_state {
  std::bitset<static_cast<std::size_t>(std::to_underlying(TButton::END) + 1)>
      buttons{};
  button::states<TButton> button_states{};
};

template <class TDriver, std::size_t CallbackCapacity, class TButton>
using button_monitor = ::jage::input::detail::monitor<
    TDriver, CallbackCapacity, button_monitor_state<TButton>, TButton,
    [](const TDriver &driver, button_monitor_state<TButton> &state) -> void {
      auto &buttons = state.buttons;

      for (auto index = 0UZ; index < std::size(buttons); ++index) {
        if (buttons[index]) {
          if (const auto button = static_cast<TButton>(index);
              driver.is_down(button)) {
            state.button_states[button].status = button::status::down;
          }
        }
      }
    },
    [](button_monitor_state<TButton> &state, const TButton button) -> void {
      state.buttons[static_cast<std::size_t>(std::to_underlying(button))] =
          true;
    },
    [](auto &callback, const button_monitor_state<TButton> &state) -> void {
      callback(state.button_states);
    },
    button::states<TButton>>;

} // namespace jage::input::detail