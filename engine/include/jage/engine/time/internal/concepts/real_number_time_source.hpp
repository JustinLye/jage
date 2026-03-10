#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <chrono>

namespace jage::time::internal::concepts {
template <class TTimeSource>
concept real_number_time_source = requires {
  requires real_number_duration<typename TTimeSource::duration>;
} and std::chrono::is_clock_v<TTimeSource>;

} // namespace jage::time::internal::concepts