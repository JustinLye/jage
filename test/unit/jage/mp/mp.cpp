#include <jage/mp/meta.hpp>
#include <jage/mp/type_of.hpp>

#include <gtest/gtest.h>

#include <type_traits>
#include <typeinfo>

using jage::mp::meta;
using jage::mp::type_of;

TEST(mp_meta, can_differentiate_meta_instantiated_with_different_types) {
  static_assert(meta<int> != meta<void>);
}

TEST(mp_meta, typeid_is_identical_accross_meta_instantiations) {
  static_assert(typeid(meta<int>) == typeid(meta<void>));
}

TEST(mp_meta, type_of_meta_is_the_same_as_instantiated_type) {
  static_assert(std::is_same_v<int, type_of<meta<int>>>);
}
