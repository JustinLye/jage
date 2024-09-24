#pragma once

#include <jage/mp/type.hpp>

namespace jage::mp {
template <class TValueType> inline constexpr auto meta = type<TValueType>::id;

} // namespace jage::mp