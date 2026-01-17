#pragma once

#include <jage/time/internal/clock.hpp>
#include <jage/time/internal/steady_clock.hpp>

namespace jage::time {

template <class TDuration>
using clock = internal::clock<internal::steady_clock<TDuration>>;

}