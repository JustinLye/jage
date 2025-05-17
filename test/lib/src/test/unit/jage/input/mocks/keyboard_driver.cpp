#include <memory>
#include <test/unit/jage/input/mocks/keyboard_driver.hpp>

std::shared_ptr<test::unit::jage::input::mocks::keyboard_driver>
    test::unit::jage::input::mocks::keyboard_driver::instance_ = nullptr;
