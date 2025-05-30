#pragma once

#include <functional>

namespace jage {
template <class TDriver> class window {
  std::reference_wrapper<TDriver> driver_;

public:
  explicit window(TDriver &driver) : driver_{driver} {}

  [[nodiscard]] constexpr auto should_close() const -> bool {
    return driver_.get().should_close();
  }

  constexpr auto poll() -> void { driver_.get().poll(); }
  constexpr auto render() -> void { driver_.get().render(); }
};
} // namespace jage
