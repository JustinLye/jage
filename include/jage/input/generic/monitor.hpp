#pragma once

#include <jage/input/generic/pollster.hpp>

#include <cstddef>
#include <ranges>

namespace jage::input::generic {

template <class TDriver, std::size_t CallbackCapacity, class TState,
          class TMonitor, auto UpdateStateFunct, auto MonitorFunct,
          auto InvokeCallbackFunct, class TCallbackParam = TState>
class monitor
    : public pollster<TDriver, CallbackCapacity, TState, UpdateStateFunct,
                      InvokeCallbackFunct, TCallbackParam> {
public:
  using monitor_target_t = TMonitor;
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