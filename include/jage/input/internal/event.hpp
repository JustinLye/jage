#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <tuple>

namespace jage::input::internal {
template <time::internal::concepts::real_number_duration TTimeDuration,
          template <class> class... TEvents>
using event = std::tuple<TEvents<TTimeDuration>...>;
} // namespace jage::input::internal