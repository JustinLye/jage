#include <jage/mp/meta.hpp>
#include <jage/mp/type_of.hpp>

#include <type_traits>
#include <typeinfo>

namespace jage::mp {
static_assert(meta<int> != meta<void>);
static_assert(typeid(meta<int>) == typeid(meta<void>));
static_assert(std::is_same_v<int, type_of<meta<int>>>);
} // namespace jage::mp

auto main(int, char *[]) -> int { return 0; }