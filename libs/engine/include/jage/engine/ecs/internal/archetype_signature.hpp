#pragma once

#include <jage/engine/ecs/internal/component_id.hpp>

#include <bitset>
#include <cassert>
#include <tuple>
#include <utility>

namespace jage::engine::ecs::internal {
template <component_id::value_type ComponentCount> class archetype_signature {
  std::bitset<ComponentCount> components_{};

public:
  [[nodiscard]] constexpr auto includes(const component_id &id) const -> bool {
    assert(id.value < ComponentCount);
    return components_.test(id.value);
  }

  [[nodiscard]] constexpr auto
  includes(const archetype_signature &signature) const noexcept -> bool {
    return signature.components_ == (components_ & signature.components_);
  }

  [[nodiscard]] constexpr auto includes(const component_id &id,
                                        auto &&...ids) const -> bool {
    return includes(id) and
           (includes(std::forward<decltype(ids)>(ids)) and ...);
  }

  constexpr auto include(const component_id &id) -> void {
    assert(id.value < ComponentCount);
    std::ignore = components_.set(id.value);
  }

  constexpr auto include(const component_id &id, auto &&...ids) -> void {
    include(id);
    (include(std::forward<decltype(ids)>(ids)), ...);
  }

  constexpr auto exclude(const component_id &id) -> void {
    assert(id.value < ComponentCount);
    std::ignore = components_.reset(id.value);
  }

  constexpr auto exclude(const component_id &id, auto &&...ids) -> void {
    exclude(id);
    (exclude(std::forward<decltype(ids)>(ids)), ...);
  }

  constexpr auto reset() -> void { components_.reset(); }
};
} // namespace jage::engine::ecs::internal