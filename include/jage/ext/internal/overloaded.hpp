#pragma once

namespace jage::ext::internal {
template <class... TCallable> struct overloaded final : TCallable... {
  using TCallable::operator()...;
};
} // namespace jage::ext::internal
