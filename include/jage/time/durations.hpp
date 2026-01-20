#pragma once

#include <chrono>

namespace jage::time::inline durations {

using seconds = std::chrono::duration<double, std::ratio<1>>;
using milliseconds = std::chrono::duration<double, std::milli>;
using microseconds = std::chrono::duration<double, std::micro>;
using nanoseconds = std::chrono::duration<double, std::nano>;

constexpr auto operator""_s(unsigned long long value) -> seconds {
  return seconds{static_cast<long double>(value)};
}

constexpr auto operator""_ms(long double value) -> milliseconds {
  return milliseconds{value};
}

constexpr auto operator""_ms(unsigned long long value) -> milliseconds {
  return milliseconds{static_cast<long double>(value)};
}

constexpr auto operator""_us(long double value) -> microseconds {
  return microseconds{value};
}

constexpr auto operator""_ns(long double value) -> nanoseconds {
  return nanoseconds{value};
}

constexpr auto operator""_ns(unsigned long long value) -> nanoseconds {
  return nanoseconds{static_cast<long double>(value)};
}

} // namespace jage::time::inline durations