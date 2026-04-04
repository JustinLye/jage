#pragma once

#include <jage/mp/first_index_of.hpp>
#include <jage/mp/list.hpp>
#include <jage/mp/unique.hpp>

#include <jage/engine/ecs/internal/component_id.hpp>

#include <optional>

namespace jage::engine::ecs::internal {

template <class... TComponents> struct component_id_registry_impl {
  static constexpr auto invalid_component_id =
      component_id{.value = sizeof...(TComponents)};

  template <class TComponent>
  [[nodiscard]] static constexpr auto get_id() -> component_id {
    return {.value = mp::first_index_of<mp::list<TComponents...>, TComponent>};
  }

  template <class TComponent>
  [[nodiscard]] static constexpr auto
  get_id(const TComponent &) -> component_id {
    return get_id<TComponent>();
  }

  template <class TComponent>
  [[nodiscard]] static constexpr auto
  try_get_id() -> std::optional<component_id> {
    if (const auto id = get_id<TComponent>(); id != invalid_component_id) {
      return {id};
    }
    return std::nullopt;
  }

  template <class TComponent>
  [[nodiscard]] static constexpr auto
  try_get_id(const TComponent &) -> std::optional<component_id> {
    return try_get_id<TComponent>();
  }
};

template <template <class...> class TList, class... TComponents>
struct component_id_registry_impl<TList<TComponents...>>
    : component_id_registry_impl<TComponents...> {};

template <class... TComponents>
using component_id_registry = component_id_registry_impl<
    typename mp::unique<mp::list<TComponents...>>::type>;

} // namespace jage::engine::ecs::internal