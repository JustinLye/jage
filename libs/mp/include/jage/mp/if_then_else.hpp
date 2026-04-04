#pragma once

namespace jage::mp {

template <bool, class TTrue, class TFalse> struct if_then_else {
  using type = TTrue;
};

template <class TTrue, class TFalse> struct if_then_else<false, TTrue, TFalse> {
  using type = TFalse;
};
} // namespace jage::mp