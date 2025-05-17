#pragma once

#include <jage/input/button/state.hpp>

#include <array>
#include <utility>

namespace jage::input::button {
template <class TButton> class states {
  std::array<state,
             static_cast<std::size_t>(std::to_underlying(TButton::END) + 1)>
      states_{};

public:
  auto operator[](const TButton button) const -> const state & {
    return states_[static_cast<std::size_t>(std::to_underlying(button))];
  }

  auto operator[](const TButton button) -> state & {
    return states_[static_cast<std::size_t>(std::to_underlying(button))];
  }
};

} // namespace jage::input::button