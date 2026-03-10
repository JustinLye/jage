#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <concepts>
#include <type_traits>

namespace jage::time::internal::concepts {

template <class TSnapshot>
concept cache_snapshot = requires(TSnapshot s) {
  requires real_number_duration<decltype(s.real_time)>;
  requires std::integral<std::remove_cvref_t<decltype(s.frame)>>;
  requires std::is_trivially_copyable_v<TSnapshot>;
  typename TSnapshot::duration;
  requires std::same_as<typename TSnapshot::duration,
                        std::remove_cvref_t<decltype(s.real_time)>>;
};

} // namespace jage::time::internal::concepts
