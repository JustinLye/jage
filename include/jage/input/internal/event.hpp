#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <variant>

namespace jage::input::internal {
template <time::internal::concepts::real_number_duration TTimeDuration,
          template <class> class... TEvents>
using event = std::variant<TEvents<TTimeDuration>...>;
} // namespace jage::input::internal