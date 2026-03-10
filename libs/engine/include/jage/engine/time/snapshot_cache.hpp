#pragma once

#include <jage/engine/concurrency/double_buffer.hpp>
#include <jage/engine/time/durations.hpp>
#include <jage/engine/time/events/snapshot.hpp>

#include <jage/engine/time/internal/snapshot_cache.hpp>

#include <atomic>

namespace jage::engine::time {
template <std::size_t Capacity,
          internal::concepts::real_number_duration TTimeDuration>
using snapshot_cache =
    internal::snapshot_cache<Capacity, events::snapshot<TTimeDuration>,
                             concurrency::double_buffer, std::atomic>;
}