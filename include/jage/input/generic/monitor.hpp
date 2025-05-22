#pragma once

#include <jage/input/generic/pollster.hpp>

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <range/v3/all.hpp>
#include <stdexcept>
#include <utility>

namespace jage::input::generic {

template <class TDriver, std::size_t CallbackCapacity, class TState,
          class TMonitor, auto UpdateStateFunct, auto MonitorFunct,
          auto InvokeCallbackFunct, class TCallbackParam = TState>
class monitor
    : public pollster<TDriver, CallbackCapacity, TState, UpdateStateFunct,
                      InvokeCallbackFunct, TCallbackParam> {
public:
  explicit monitor(TDriver &driver)
      : pollster<TDriver, CallbackCapacity, TState, UpdateStateFunct,
                 InvokeCallbackFunct, TCallbackParam>{driver} {}

  auto monitor_input(const auto... targets) -> void {
    (MonitorFunct(this->state_, static_cast<TMonitor>(targets)), ...);
  }

  auto monitor_input(const std::ranges::range auto &targets) -> void {
    for (const auto &target : targets) {
      MonitorFunct(this->state_, target);
    }
  }
};

} // namespace jage::input::generic