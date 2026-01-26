#pragma once

#include <jage/memory/cacheline_size.hpp>

#include <cstdint>

namespace jage::time::internal::events {
template <class TDuration> struct alignas(memory::cacheline_size) snapshot {
  TDuration real_time{};
  TDuration tick_duration{};
  double time_scale{1.0};
  TDuration elapsed_time{};
  std::uint64_t elapsed_ticks{};
  std::uint64_t ticks{};
  TDuration accumulated_time{};
};
} // namespace jage::time::internal::events