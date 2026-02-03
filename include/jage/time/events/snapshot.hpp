#pragma once

#include <jage/memory/cacheline_size.hpp>

#include <cstdint>

namespace jage::time::events {
template <class TDuration> struct alignas(memory::cacheline_size) snapshot {
  using duration = TDuration;
  TDuration real_time{};
  TDuration tick_duration{};
  double time_scale{1.0};
  TDuration elapsed_time{};
  std::uint64_t elapsed_frames{};
  std::uint64_t frame{};
  TDuration accumulated_time{};
  auto operator<=>(const snapshot &) const = default;
};
} // namespace jage::time::events