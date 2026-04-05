#include <jage/engine/ecs/archetype_signature.hpp>

#include <jage/engine/ecs/internal/component_id.hpp>

#include <gtest/gtest.h>

using jage::engine::ecs::archetype_signature;
using jage::engine::ecs::internal::component_id;

static constexpr auto component_0 = component_id{.value = 0};
static constexpr auto component_1 = component_id{.value = 1};
static constexpr auto component_2 = component_id{.value = 2};

TEST(jage_engine_ecs_achetype_signature_test,
     should_detect_when_component_is_added) {

  auto signature = archetype_signature<1>{};
  EXPECT_FALSE(signature.has(component_0));
  signature.add_component(component_0);
  EXPECT_TRUE(signature.has(component_0));
}

TEST(jage_engine_ecs_archetype_signature_test,
     should_detect_when_component_is_removed) {

  auto signature = archetype_signature<1>{};
  signature.add_component(component_0);
  signature.remove_component(component_0);
  EXPECT_FALSE(signature.has(component_0));
}

TEST(jage_engine_ecs_archetype_signature_test, should_have_reset) {
  auto signature = archetype_signature<1>{};

  signature.add_component(component_0);
  signature.reset();
  EXPECT_FALSE(signature.has(component_0));
}

TEST(jage_engine_ecs_archetype_signature_test,
     should_allow_adding_and_removing_variable_number) {
  auto signature = archetype_signature<3>{};

  signature.add_component(component_0, component_2, component_1);
  EXPECT_TRUE(signature.has(component_0));
  EXPECT_TRUE(signature.has(component_1));
  EXPECT_TRUE(signature.has(component_2));
  signature.remove_component(component_1, component_2);
  EXPECT_TRUE(signature.has(component_0));
  EXPECT_FALSE(signature.has(component_1));
  EXPECT_FALSE(signature.has(component_2));
}

TEST(jage_engine_ecs_archetype_signature_test,
     should_allow_checking_variable_number) {
  auto signature = archetype_signature<3>{};

  signature.add_component(component_0, component_1, component_2);
  EXPECT_TRUE(signature.has(component_0, component_2));
  EXPECT_TRUE(signature.has(component_0, component_2, component_1));
  signature.remove_component(component_0);
  EXPECT_FALSE(signature.has(component_0, component_2, component_1));
}

TEST(jage_engine_ecs_archetype_signature_test,
     should_allow_checking_for_all_components_in_another_archetype) {
  auto signature_0 = archetype_signature<3>{};
  auto signature_1 = archetype_signature<3>{};

  signature_0.add_component(component_0, component_1);
  signature_1.add_component(component_0, component_1);

  EXPECT_TRUE(signature_0.contains_all_components_in(signature_1));
  EXPECT_TRUE(signature_1.contains_all_components_in(signature_0));

  signature_0.add_component(component_2);

  EXPECT_TRUE(signature_0.contains_all_components_in(signature_1));
  EXPECT_FALSE(signature_1.contains_all_components_in(signature_0));
}
