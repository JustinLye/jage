#pragma once

#include <jage/engine/ext/internal/hardware_interference_size.hpp>

namespace jage::engine::memory {
static_assert(ext::internal::hardware_constructive_interference_size ==
              ext::internal::hardware_destructive_interference_size);

static constexpr auto cacheline_size =
    ext::internal::hardware_constructive_interference_size;

} // namespace jage::engine::memory