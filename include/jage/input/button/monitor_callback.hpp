#pragma once

namespace jage::input::button {
static constexpr auto monitor_callback = [](auto &callback,
                                            const auto &state) -> void {
  callback(state.button_states);
};
} // namespace jage::input::button