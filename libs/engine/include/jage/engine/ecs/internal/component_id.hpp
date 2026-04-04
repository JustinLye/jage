#pragma once

#include <compare>
#include <cstdint>

namespace jage::engine::ecs::internal {
struct component_id {
  using value_type = std::uint16_t;

  value_type value;

  [[nodiscard]] auto
  operator==(const component_id &) const noexcept -> bool = default;
  [[nodiscard]] auto operator<=>(const component_id &) const noexcept
      -> std::strong_ordering = default;
};
} // namespace jage::engine::ecs::internal