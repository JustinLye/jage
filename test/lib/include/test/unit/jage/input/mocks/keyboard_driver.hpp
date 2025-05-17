#pragma once

#include <jage/input/keys.hpp>

#include <gmock/gmock.h>

#include <memory>

namespace test::unit::jage::input::mocks {
class keyboard_driver {
  static std::shared_ptr<keyboard_driver> instance_;

public:
  static auto instance() -> keyboard_driver & {
    if (not instance_) {
      instance_ = std::make_shared<keyboard_driver>();
    }
    return *instance_;
  }
  static auto reset() -> void { instance_.reset(); }

  MOCK_METHOD(bool, is_down_, (const ::jage::input::keys));
  static auto is_down(const ::jage::input::keys key) -> bool {
    return instance().is_down_(key);
  }
};
} // namespace test::unit::jage::input::mocks