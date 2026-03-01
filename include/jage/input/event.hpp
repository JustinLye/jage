#pragma once

#include <jage/input/keyboard/events/key_press.hpp>
#include <jage/input/mouse/events/click.hpp>
#include <jage/input/mouse/events/cursor/motion.hpp>
#include <jage/input/mouse/events/cursor/position.hpp>
#include <jage/input/mouse/events/horizontal_scroll.hpp>
#include <jage/input/mouse/events/vertical_scroll.hpp>

#include <jage/input/internal/event.hpp>
#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input {
template <time::internal::concepts::real_number_duration TTimeDuration>
using event = internal::event<
    TTimeDuration, keyboard::events::key_press, mouse::events::click,
    mouse::events::cursor::position, mouse::events::cursor::motion,
    mouse::events::horizontal_scroll, mouse::events::vertical_scroll>;
}