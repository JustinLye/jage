#pragma once

#include <jage/engine/time/internal/concepts/real_number_duration.hpp>

#include <chrono>

namespace jage::engine::time::internal::concepts {
template <class TTimeSource>
concept real_number_time_source = requires {
  requires real_number_duration<typename TTimeSource::duration>;
} and std::chrono::is_clock_v<TTimeSource>;

} // namespace jage::engine::time::internal::concepts