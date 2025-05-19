#pragma once

#include <jage/input/cursor/state.hpp>
#include <jage/input/generic/monitor.hpp>

#include <cstddef>

namespace jage::input::cursor {

static constexpr auto update_state =
    []<class TDriver>(const TDriver &driver,
                      jage::input::cursor::state &state) -> void {
  state = driver.cursor_position();
};

template <class TDriver, std::size_t CallbackCapacity = 2>
using monitor =
    generic::monitor<TDriver, CallbackCapacity, state, std::uint8_t,
                     update_state, [](const std::uint8_t &) -> void {},
                     [](const auto &callback,
                        const jage::input::cursor::state &state) -> void {
                       callback(state);
                     }>;

} // namespace jage::input::cursor