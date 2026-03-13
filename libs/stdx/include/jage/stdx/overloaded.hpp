#pragma once

namespace jage::stdx {
template <class... TCallable> struct overloaded final : TCallable... {
  using TCallable::operator()...;
};
} // namespace jage::stdx
