#pragma once

#include <jage/input/button/monitor.hpp>
#include <jage/input/keyboard/key.hpp>

namespace jage::input::keyboard {

template <class TDriver, std::size_t CallbackCapacity = 2>
using monitor = ::jage::input::button::monitor<TDriver, CallbackCapacity, key>;

} // namespace jage::input::keyboard