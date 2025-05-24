#pragma once

#include <jage/window.hpp>

#include <jage/test/mocks/window/driver.hpp>

#include <gtest/gtest.h>

namespace jage::test::fixtures {
struct window_test : public testing::Test {
protected:
  mocks::window::driver driver{};
  jage::window<jage::test::mocks::window::driver> window{driver};
};

} // namespace jage::test::fixtures
