#pragma once

#include <jage/input/keyboard/keys.hpp>

#include <jage/test/fixtures/input/input_keyboard_monitoring.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input {
struct parameterized_input_keyboard_monitoring
    : input_keyboard_monitoring,
      testing::WithParamInterface<
          std::underlying_type_t<jage::input::keyboard::keys>> {};
} // namespace jage::test::fixtures::input