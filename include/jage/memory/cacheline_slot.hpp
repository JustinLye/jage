#pragma once

#include <jage/memory/cacheline_size.hpp>

#include <array>
#include <cstddef>
#include <type_traits>
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
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
          std::is_arithmetic_v<TUnderlyingValue>
class alignas(cacheline_size) cacheline_slot<TUnderlyingValue> final {
  TUnderlyingValue value_;
  [[no_unique_address]] std::array<std::byte,
                                   cacheline_size - (sizeof(TUnderlyingValue) %
                                                     cacheline_size)> padding_;

public:
  cacheline_slot(auto &&...args)
      : value_{std::forward<decltype(args)>(args)...} {}

  operator TUnderlyingValue() const { return value_; }

  operator TUnderlyingValue() { return value_; }
};

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
          (not std::is_arithmetic_v<TUnderlyingValue>)
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
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
              std::is_arithmetic_v<TUnderlyingValue>
cacheline_slot(TUnderlyingValue &&) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
              std::is_arithmetic_v<TUnderlyingValue>
cacheline_slot(TUnderlyingValue &) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
              (not std::is_arithmetic_v<TUnderlyingValue>)
cacheline_slot(TUnderlyingValue &&) -> cacheline_slot<TUnderlyingValue>;

template <class TUnderlyingValue>
  requires(sizeof(TUnderlyingValue) < cacheline_size) and
              (not std::is_arithmetic_v<TUnderlyingValue>)
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