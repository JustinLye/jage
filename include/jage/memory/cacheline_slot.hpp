#pragma once

#include <jage/memory/cacheline_size.hpp>

#include <array>
#include <cstddef>
#include <utility>

namespace jage::memory {

template <class TUnderlyingValue>
class alignas(cacheline_size) cacheline_slot final : public TUnderlyingValue {
  [[no_unique_address]] std::array<std::byte,
                                   cacheline_size - (sizeof(TUnderlyingValue) %
                                                     cacheline_size)> padding_;

public:
  cacheline_slot(auto &&...args)
      : TUnderlyingValue(std::forward<decltype(args)>(args)...) {}
};

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size)
class alignas(cacheline_size) cacheline_slot<TUnderlyingValue> final
    : public TUnderlyingValue {
  [[no_unique_address]] std::array<
      std::byte, (cacheline_size - sizeof(TUnderlyingValue))> padding_;

public:
  cacheline_slot(auto &&...args)
      : TUnderlyingValue(std::forward<decltype(args)>(args)...) {}
};

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) % cacheline_size == 0UZ)
class alignas(cacheline_size) cacheline_slot<TUnderlyingValue> final
    : public TUnderlyingValue {
public:
  cacheline_slot(auto &&...args)
      : TUnderlyingValue(std::forward<decltype(args)>(args)...) {}
};

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size)
cacheline_slot(TUnderlyingValue &&) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size)
cacheline_slot(TUnderlyingValue &) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) % cacheline_size == 0UZ)
cacheline_slot(TUnderlyingValue &) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) % cacheline_size == 0UZ)
cacheline_slot(TUnderlyingValue &&) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
cacheline_slot(TUnderlyingValue &) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
cacheline_slot(TUnderlyingValue &&) -> cacheline_slot<TUnderlyingValue>;

} // namespace jage::memory