#pragma once

#include <jage/mp/internal/unique_values.hpp>

namespace jage::mp::concepts {

template <auto... Vs>
concept unique_values = internal::unique_values<Vs...>;
}