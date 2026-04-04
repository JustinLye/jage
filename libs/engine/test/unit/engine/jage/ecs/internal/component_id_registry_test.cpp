#include <jage/engine/ecs/internal/component_id.hpp>
#include <jage/engine/ecs/internal/component_id_registry.hpp>

#include <gtest/gtest.h>

using jage::engine::ecs::internal::component_id;
using jage::engine::ecs::internal::component_id_registry;

struct foo {};
struct bar {};
struct baz {};

TEST(component_id_registry,
     should_make_the_invalid_id_the_size_of_the_component_params) {
  EXPECT_EQ(component_id{.value = 1},
            component_id_registry<foo>::invalid_component_id);
  EXPECT_EQ(component_id{.value = 2},
            (component_id_registry<foo, bar>::invalid_component_id));
}

TEST(component_id_registry_get_id,
     should_register_id_using_index_of_component_parameters) {

  using registry_type = component_id_registry<foo, bar>;
  auto registry = registry_type{};
  {
    constexpr auto expected_component_id = component_id{.value = 0};
    const auto component = foo{};

    static_assert(expected_component_id == registry.get_id<foo>());
    static_assert(expected_component_id == registry.get_id(foo{}));
    static_assert(expected_component_id == registry.get_id(component));
  }

  {
    constexpr auto expected_component_id = component_id{.value = 1};
    const auto component = bar{};

    static_assert(expected_component_id == registry.get_id<bar>());
    static_assert(expected_component_id == registry.get_id(bar{}));
    static_assert(expected_component_id == registry.get_id(component));
  }
}

TEST(component_id_registry_get_id,
     should_return_invalid_id_component_id_is_not_registered) {
  using registry_type = component_id_registry<bar>;
  constexpr auto expected_component_id = registry_type::invalid_component_id;
  auto registry = registry_type{};
  const auto component = foo{};
  static_assert(expected_component_id == registry.get_id<foo>());
  static_assert(expected_component_id == registry.get_id(foo{}));
  static_assert(expected_component_id == registry.get_id(component));
}

TEST(component_id_registry_try_get_id,
     should_register_id_using_index_of_component_parameters) {
  using registry_type = component_id_registry<foo, bar>;
  auto registry = registry_type{};
  {
    constexpr auto expected_component_id = component_id{.value = 0};
    const auto component = foo{};

    static_assert(expected_component_id == registry.try_get_id<foo>().value());
    static_assert(expected_component_id == registry.try_get_id(foo{}).value());
    static_assert(expected_component_id ==
                  registry.try_get_id(component).value());
  }

  {
    constexpr auto expected_component_id = component_id{.value = 1};
    const auto component = bar{};

    static_assert(expected_component_id == registry.try_get_id<bar>().value());
    static_assert(expected_component_id == registry.try_get_id(bar{}).value());
    static_assert(expected_component_id ==
                  registry.try_get_id(component).value());
  }
}

TEST(component_id_registry_sanitized,
     should_remove_duplicate_before_assigning_component_ids) {
  using registry_type = component_id_registry<bar, foo, bar, baz>;
  auto registry = registry_type{};

  static_assert(component_id{.value = 3} ==
                registry_type::invalid_component_id);
  static_assert(component_id{.value = 0} == registry.get_id<bar>());
  static_assert(component_id{.value = 1} == registry.get_id<foo>());
  static_assert(component_id{.value = 2} == registry.get_id<baz>());
}

TEST(component_id_registry_special, should_not_unwrap_templated_types) {
  using registry_type = component_id_registry<std::tuple<int, bool>>;

  auto registry = registry_type{};

  static_assert(component_id{.value = 1} ==
                registry_type::invalid_component_id);
  static_assert(component_id{.value = 0} ==
                registry.get_id<std::tuple<int, bool>>());
}