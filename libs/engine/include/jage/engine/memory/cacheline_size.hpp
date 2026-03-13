#pragma once

#include <jage/stdx/hardware_interference_size.hpp>

namespace jage::engine::memory {
static_assert(stdx::hardware_constructive_interference_size ==
              stdx::hardware_destructive_interference_size);

static constexpr auto cacheline_size =
    stdx::hardware_constructive_interference_size;

} // namespace jage::engine::memory