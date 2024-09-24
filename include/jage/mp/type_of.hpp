#pragma once

#include <jage/mp/info.hpp>
#include <jage/mp/meta.hpp>
#include <jage/mp/type.hpp>

namespace jage::mp {
template <auto meta>
using type_of = typename decltype(get(info<meta>{}))::value_type;
}