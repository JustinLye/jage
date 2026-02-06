#pragma once

#include <jage/concurrency/double_buffer.hpp>

#include <jage/containers/spmc/internal/ring_buffer.hpp>

#include <atomic>
#include <cstddef>

namespace jage::containers::spmc {
template <class TEvent, std::size_t Capacity>
using ring_buffer = internal::ring_buffer<TEvent, Capacity, std::atomic,
                                          concurrency::double_buffer>;
}