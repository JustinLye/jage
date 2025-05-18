#pragma once

#include <jage/input/button/monitor_button.hpp>
#include <jage/input/button/monitor_callback.hpp>
#include <jage/input/button/monitor_state.hpp>
#include <jage/input/button/state.hpp>
#include <jage/input/button/states.hpp>
#include <jage/input/button/status.hpp>
#include <jage/input/button/update_states.hpp>
#include <jage/input/generic/monitor.hpp>

namespace jage::input::button {

template <class TDriver, std::size_t CallbackCapacity, class TButton>
using monitor = ::jage::input::generic::monitor<
    TDriver, CallbackCapacity, monitor_state<TButton>, TButton, update_states,
    monitor_button, monitor_callback, button::states<TButton>>;

} // namespace jage::input::button