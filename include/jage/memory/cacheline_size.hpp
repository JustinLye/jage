#pragma once

#include <jage/ext/internal/hardware_interference_size.hpp>

namespace jage::memory {
static_assert(ext::internal::hardware_constructive_interference_size ==
              ext::internal::hardware_destructive_interference_size);

static constexpr auto cacheline_size =
    ext::internal::hardware_constructive_interference_size;

} // namespace jage::memory