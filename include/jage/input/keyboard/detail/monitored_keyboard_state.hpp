#pragma once

#include <jage/input/keyboard/keys.hpp>
#include <jage/input/keyboard/state.hpp>

#include <bitset>
#include <cstddef>
#include <utility>

namespace jage::input::keyboard::detail {
struct monitored_keyboard_state {
  std::bitset<static_cast<std::size_t>(std::to_underlying(keys::END) + 1)>
      monitored_keys{};
  state keyboard_state{};
};
} // namespace jage::input::keyboard::detail