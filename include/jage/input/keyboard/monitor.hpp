#pragma once

#include <jage/input/detail/button_monitor.hpp>
#include <jage/input/keyboard/keys.hpp>

namespace jage::input::keyboard {

template <class TDriver, std::size_t CallbackCapacity = 2>
using monitor =
    ::jage::input::detail::button_monitor<TDriver, CallbackCapacity, keys>;

} // namespace jage::input::keyboard