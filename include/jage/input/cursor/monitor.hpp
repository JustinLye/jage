#pragma once

#include <jage/input/cursor/state.hpp>
#include <jage/input/generic/pollster.hpp>

#include <cstddef>

namespace jage::input::cursor {

static constexpr auto update_state =
    []<class TDriver>(const TDriver &driver,
                      jage::input::cursor::state &state) -> void {
  state = driver.cursor_position();
};

static constexpr auto invoke_callback =
    [](const auto &callback, const jage::input::cursor::state &state) -> void {
  callback(state);
};

template <class TDriver, std::size_t CallbackCapacity = 2>
struct monitor
    : public generic::pollster<TDriver, CallbackCapacity, state, update_state,
                               invoke_callback, state> {
  explicit monitor(TDriver &driver)
      : generic::pollster<TDriver, CallbackCapacity, state, update_state,
                          invoke_callback, state>{driver} {}
};

} // namespace jage::input::cursor