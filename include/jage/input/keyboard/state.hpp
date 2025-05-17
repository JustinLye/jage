#pragma once

#include <jage/input/keyboard/keys.hpp>
#include <jage/input/status.hpp>

#include <array>
#include <cstddef>
#include <utility>

namespace jage::input::keyboard {
struct key_state {
  status status{status::up};
};

class state {
  std::array<key_state,
             static_cast<std::size_t>(std::to_underlying(keys::END) + 1)>
      states_{};

public:
  auto operator[](const keys key) const -> const key_state & {
    return states_[static_cast<std::size_t>(std::to_underlying(key))];
  }

  auto operator[](const keys key) -> key_state & {
    return states_[static_cast<std::size_t>(std::to_underlying(key))];
  }
};
} // namespace jage::input::keyboard