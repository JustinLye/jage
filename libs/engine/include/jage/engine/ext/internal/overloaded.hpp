#pragma once

namespace jage::engine::ext::internal {
template <class... TCallable> struct overloaded final : TCallable... {
  using TCallable::operator()...;
};
} // namespace jage::engine::ext::internal
