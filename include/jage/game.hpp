#pragma once

#include <jage/input/controller.hpp>
#include <jage/window.hpp>

#include <functional>

namespace jage {
template <class TWindow, class TInputController> class game {
  std::reference_wrapper<TWindow> window_;
  std::reference_wrapper<TInputController> input_controller_;

public:
  explicit game(TWindow &window, TInputController &input_controller)
      : window_{window}, input_controller_{input_controller} {}

  auto loop() -> void {
    while (not window_.get().should_close()) {
      input_controller_.get().poll();
      window_.get().poll();
    }
  }

  [[nodiscard]] auto input() -> TInputController & {
    return input_controller_.get();
  }
};
} // namespace jage
