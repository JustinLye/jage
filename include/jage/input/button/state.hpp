#pragma once

#include <jage/input/button/status.hpp>

namespace jage::input::button {

struct state {
  jage::input::button::status status{status::up};
};
} // namespace jage::input::button