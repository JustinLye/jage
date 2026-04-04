#pragma once

#include <jage/mp/internal/first_index_of.hpp>

namespace jage::mp {

template <class TList, class TTarget>
static constexpr auto first_index_of =
    internal::first_index_of<TList, TTarget>::value;

} // namespace jage::mp