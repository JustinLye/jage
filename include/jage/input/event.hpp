#pragma once

#include <jage/input/keyboard/event.hpp>
#include <jage/input/mouse/events/click.hpp>

#include <jage/input/internal/event.hpp>
#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input {
template <time::internal::concepts::real_number_duration TTimeDuration>
using event =
    internal::event<TTimeDuration, keyboard::event, mouse::events::click>;
}