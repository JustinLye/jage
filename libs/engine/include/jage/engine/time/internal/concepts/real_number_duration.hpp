#pragma once

#include <type_traits>

namespace jage::engine::time::internal::concepts {
template <class TTimeDuration>
concept real_number_duration = requires {
  typename TTimeDuration::rep;
  typename TTimeDuration::period;
  requires std::is_floating_point_v<typename TTimeDuration::rep>;
};
} // namespace jage::engine::time::internal::concepts