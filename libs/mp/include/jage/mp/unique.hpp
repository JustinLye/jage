#pragma once
#include <jage/mp/internal/unique.hpp>

namespace jage::mp {
template <class TList> struct unique {
  using type = internal::unique<TList>::type;
};

} // namespace jage::mp