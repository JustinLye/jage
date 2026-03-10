#pragma once

#include <jage/engine/time/internal/clock.hpp>
#include <jage/engine/time/internal/steady_clock.hpp>

namespace jage::engine::time {

template <class TDuration>
using clock = internal::clock<internal::steady_clock<TDuration>>;

}