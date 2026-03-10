#pragma once

#include <jage/engine/input/keyboard/events/key_press.hpp>
#include <jage/engine/input/mouse/events/click.hpp>
#include <jage/engine/input/mouse/events/cursor/motion.hpp>
#include <jage/engine/input/mouse/events/cursor/position.hpp>
#include <jage/engine/input/mouse/events/horizontal_scroll.hpp>
#include <jage/engine/input/mouse/events/vertical_scroll.hpp>

#include <jage/engine/input/internal/event.hpp>
#include <jage/engine/time/internal/concepts/real_number_duration.hpp>

namespace jage::engine::input {
template <time::internal::concepts::real_number_duration TTimeDuration>
using event = internal::event<
    TTimeDuration, keyboard::events::key_press, mouse::events::click,
    mouse::events::cursor::position, mouse::events::cursor::motion,
    mouse::events::horizontal_scroll, mouse::events::vertical_scroll>;
}