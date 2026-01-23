#pragma once

#include <cstdint> // IWYU pragma: keep
#include <new>     // IWYU pragma: keep

namespace jage::ext::internal {
static constexpr auto hardware_constructive_interference_size = 64UZ;
static constexpr auto hardware_destructive_interference_size = 64UZ;
#if defined(__cpp_lib_hardware_interference_size) and                          \
    __cpp_lib_hardware_interference_size >= 201703L
#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winterference-size"
#endif
static_assert(hardware_constructive_interference_size ==
              std::hardware_constructive_interference_size);
static_assert(hardware_destructive_interference_size ==
              std::hardware_destructive_interference_size);
#if defined(__GNUC__) and !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#endif
} // namespace jage::ext::internal