#pragma once

#include <jage/input/keyboard/monitor.hpp>

#include <jage/test/fixtures/input/monitoring.hpp>
#include <jage/test/mocks/input/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures::input::keyboard {
using monitoring = ::jage::test::fixtures::input::monitoring<
    ::jage::input::keyboard::monitor<
        mocks::input::driver<::jage::input::keyboard::keys>>,
    ::jage::input::keyboard::keys>;

} // namespace jage::test::fixtures::input::keyboard