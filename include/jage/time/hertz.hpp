#pragma once

#include <jage/time/durations.hpp>

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <concepts>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace jage::time {
class hertz {
  using value_type_ = std::uint16_t;
  value_type_ cycles_;
  template <internal::concepts::real_number_duration TTimeDuration>
  constexpr auto to_duration() const -> TTimeDuration {
    constexpr auto period_duration =
        static_cast<double>(TTimeDuration::period::den);
    return TTimeDuration{period_duration / cycles_};
  }

public:
  using value_type = value_type_;

  template <std::integral T>
  explicit constexpr hertz(const T cycles)
      : cycles_{static_cast<value_type>(cycles)} {
    if (0 >= cycles) {
      throw std::invalid_argument("Refusing to construct hertz with cycles "
                                  "less than or equal to zero.");
    }
    if (std::numeric_limits<hertz::value_type>::max() < cycles) [[unlikely]] {
      throw std::invalid_argument("Refusing to construct hertz with cycles "
                                  "that would overflow value type");
    }
  }

  template <internal::concepts::real_number_duration TTimeDuration>
  constexpr explicit operator TTimeDuration() const {
    return to_duration<TTimeDuration>();
  }
};

constexpr auto operator""_Hz(unsigned long long value) -> hertz {
  if (std::numeric_limits<hertz::value_type>::max() < value) [[unlikely]] {
    throw std::invalid_argument("Refusing to construct hertz with cycles "
                                "that would overflow value type");
  }
  return hertz(static_cast<hertz::value_type>(value));
}
} // namespace jage::time