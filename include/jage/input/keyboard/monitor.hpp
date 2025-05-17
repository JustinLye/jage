#pragma once

#include <jage/input/detail/monitor.hpp>
#include <jage/input/keyboard/detail/monitored_keyboard_state.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/keyboard/state.hpp>

#include <cstddef>
#include <utility>

namespace jage::input::keyboard {

template <class TDriver, std::size_t CallbackCapacity = 2>
using monitor = ::jage::input::detail::monitor<
    TDriver, CallbackCapacity, detail::monitored_keyboard_state, keys,
    [](const auto &driver, detail::monitored_keyboard_state &state) -> void {
      auto &monitored_keys = state.monitored_keys;

      for (auto index = 0UZ; index < std::size(monitored_keys); ++index) {
        if (monitored_keys[index]) {
          if (const auto key = static_cast<keys>(index);
              driver.get().is_down(key)) {
            state.keyboard_state[key].status = status::down;
          }
        }
      }
    },
    [](detail::monitored_keyboard_state &state, const keys key) -> void {
      state.monitored_keys[static_cast<std::size_t>(std::to_underlying(key))] =
          true;
    },
    [](auto &callback, const detail::monitored_keyboard_state &state) -> void {
      callback(state.keyboard_state);
    },
    state>;

} // namespace jage::input::keyboard