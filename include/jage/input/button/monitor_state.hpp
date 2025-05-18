#pragma once
#include <jage/input/button/states.hpp>

#include <bitset>
#include <cstddef>
#include <utility>

namespace jage::input::button {
template <class TButton> struct monitor_state {
  using button_type = TButton;
  std::bitset<static_cast<std::size_t>(std::to_underlying(TButton::END) + 1)>
      buttons{};
  states<TButton> button_states{};
};
} // namespace jage::input::button