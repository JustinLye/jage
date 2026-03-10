#pragma once

#include <jage/concurrency/double_buffer.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/time/internal/snapshot_cache.hpp>

#include <atomic>

namespace jage::time {
template <std::size_t Capacity,
          internal::concepts::real_number_duration TTimeDuration>
using snapshot_cache =
    internal::snapshot_cache<Capacity, events::snapshot<TTimeDuration>,
                             concurrency::double_buffer, std::atomic>;
}