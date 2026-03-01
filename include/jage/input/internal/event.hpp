#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <variant>

namespace jage::input::internal {
template <time::internal::concepts::real_number_duration TTimeDuration,
          class... TPayloads>
struct event {
  using payload_type = std::variant<TPayloads...>;
  TTimeDuration timestamp;
  payload_type payload;
};
} // namespace jage::input::internal