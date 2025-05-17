#pragma once

#include <jage/input/key_status.hpp>
#include <jage/input/keys.hpp>

#include <array>
#include <cstddef>
#include <utility>

namespace jage::input {
struct key_state {
  key_status status{key_status::up};
};

class keyboard_state {
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
} // namespace jage::input