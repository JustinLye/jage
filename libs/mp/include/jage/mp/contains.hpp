#pragma once

#include <jage/mp/internal/contains.hpp>

namespace jage::mp {
template <class TList, class TTarget>
static constexpr auto contains = internal::contains<TTarget, TList>;

} // namespace jage::mp