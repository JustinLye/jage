#pragma once
#include <jage/mp/internal/unique.hpp>

namespace jage::mp {
template <class... Ts> struct unique {
  using type = internal::unique<Ts...>::type;
};

} // namespace jage::mp