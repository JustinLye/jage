#pragma once

#include <jage/input/button/state.hpp>
#include <jage/input/button/states.hpp>
#include <jage/input/button/status.hpp>
#include <jage/input/detail/monitor.hpp>

#include <bitset>
#include <cstddef>
#include <utility>

namespace jage::input::button {

template <class TButton> struct button_monitor_state {
  using button_type = TButton;
  std::bitset<static_cast<std::size_t>(std::to_underlying(TButton::END) + 1)>
      buttons{};
  button::states<TButton> button_states{};
};

static constexpr auto button_monitor_update_button_states =
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

static constexpr auto monitor_button = [](auto &state,
                                          const auto button) -> void {
  state.buttons[static_cast<std::size_t>(std::to_underlying(button))] = true;
};

static constexpr auto invoke_callback_with_button_states =
    [](auto &callback, const auto &state) -> void {
  callback(state.button_states);
};

template <class TDriver, std::size_t CallbackCapacity, class TButton>
using monitor = ::jage::input::detail::monitor<
    TDriver, CallbackCapacity, button_monitor_state<TButton>, TButton,
    button_monitor_update_button_states, monitor_button,
    invoke_callback_with_button_states, button::states<TButton>>;

} // namespace jage::input::button