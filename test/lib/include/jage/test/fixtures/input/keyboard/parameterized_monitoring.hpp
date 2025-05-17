#pragma once

#include <jage/input/keyboard/keys.hpp>

#include <jage/test/fixtures/input/keyboard/monitoring.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input::keyboard {
struct parameterized_monitoring
    : monitoring,
      testing::WithParamInterface<
          std::underlying_type_t<jage::input::keyboard::keys>> {};
} // namespace jage::test::fixtures::input::keyboard