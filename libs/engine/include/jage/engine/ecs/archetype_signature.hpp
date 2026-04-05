#pragma once

#include <jage/engine/ecs/internal/component_id.hpp>

#include <bitset>
#include <cassert>
#include <tuple>
#include <utility>

namespace jage::engine::ecs {
template <internal::component_id::value_type ComponentCount>
class archetype_signature {
  std::bitset<ComponentCount> components_{};

public:
  [[nodiscard]] constexpr auto
  has(const internal::component_id &id) const -> bool {
    assert(id.value < ComponentCount);
    return components_.test(id.value);
  }

  [[nodiscard]] constexpr auto contains_all_components_in(
      const archetype_signature &signature) const noexcept -> bool {
    return signature.components_ == (components_ & signature.components_);
  }

  [[nodiscard]] constexpr auto has(const internal::component_id &id,
                                   auto &&...ids) const -> bool {
    return has(id) and (has(std::forward<decltype(ids)>(ids)) and ...);
  }

  constexpr auto add_component(const internal::component_id &id) -> void {
    assert(id.value < ComponentCount);
    std::ignore = components_.set(id.value);
  }

  constexpr auto add_component(const internal::component_id &id,
                               auto &&...ids) -> void {
    add_component(id);
    (add_component(std::forward<decltype(ids)>(ids)), ...);
  }

  constexpr auto remove_component(const internal::component_id &id) -> void {
    assert(id.value < ComponentCount);
    std::ignore = components_.reset(id.value);
  }

  constexpr auto remove_component(const internal::component_id &id,
                                  auto &&...ids) -> void {
    remove_component(id);
    (remove_component(std::forward<decltype(ids)>(ids)), ...);
  }

  constexpr auto reset() -> void { components_.reset(); }
};
} // namespace jage::engine::ecs