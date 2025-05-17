#pragma once

#include <jage/input/detail/button_monitor.hpp>
#include <jage/input/mouse/buttons.hpp>

namespace jage::input::mouse {

template <class TDriver, std::size_t CallbackCapacity = 2>
using monitor =
    ::jage::input::detail::button_monitor<TDriver, CallbackCapacity, buttons>;

} // namespace jage::input::mouse