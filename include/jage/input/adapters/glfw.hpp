#pragma once

#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/event.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/scan_code.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <vector>

namespace jage::input::adapters {
// TODO: Make this a concept
template <class TPlatform> class glfw {

  static std::array<keyboard::key, GLFW_KEY_LAST> logical_keys_;
  static std::vector<keyboard::scan_code> physical_keys_;

  static constexpr auto get_physical_key =
      [](const int scancode) -> keyboard::scan_code {
    if (0 > scancode) [[unlikely]] {
      return keyboard::scan_code::unidentified;
    } else if (static_cast<std::size_t>(scancode) >= std::size(physical_keys_))
        [[unlikely]] {
      return keyboard::scan_code::unidentified;
    } else [[likely]] {
      return physical_keys_[static_cast<std::size_t>(scancode)];
    }
  };

  static constexpr auto key_callback =
      [](typename TPlatform::window_handler_pointer_type window, int key,
         int scancode, int, int) -> void {
    auto &context = *static_cast<typename TPlatform::context_type *>(
        TPlatform::get_window_user_pointer(window));
    context.push(typename TPlatform::context_type::event_type{
        keyboard::event<typename TPlatform::context_type::duration_type>{
            .timestamp = typename TPlatform::context_type::duration_type{},
            .key = logical_keys_[key], // NEED TO TEST NON-MAPPED
            .scan_code = get_physical_key(
                scancode), // NEED TO HAVE EXPLICIT TEST FOR THIS
            .action = keyboard::action::press,
            .modifiers = 0,
        }});
  };

  static constexpr auto load_physical_scancodes =
      [](const TPlatform &platform) -> void {
    auto bind = [&](const auto glfw_key,
                    const keyboard::scan_code scancode) -> void {
      if (const auto os_scancode = platform.get_key_scancode(glfw_key);
          os_scancode != -1) {
        if (static_cast<std::size_t>(os_scancode) >=
            std::size(physical_keys_)) {
          physical_keys_.resize(static_cast<std::size_t>(os_scancode) + 1UZ,
                                keyboard::scan_code::unidentified);
        }
        physical_keys_[os_scancode] = scancode;
      }
    };

    bind(GLFW_KEY_A, keyboard::scan_code::a);
    bind(GLFW_KEY_ESCAPE, keyboard::scan_code::escape);
  };

  static constexpr auto load_logical_keys = [] -> void {
    logical_keys_[GLFW_KEY_A] = keyboard::key::a;
    logical_keys_[GLFW_KEY_ESCAPE] = keyboard::key::escape;
  };

public:
  glfw(typename TPlatform::window_handler_pointer_type window,
       const TPlatform &platform) {
    load_logical_keys();
    load_physical_scancodes(platform);
    platform.set_key_callback(window, key_callback);
  }
};

template <class TPlatform>
std::array<keyboard::key, GLFW_KEY_LAST> glfw<TPlatform>::logical_keys_ = {};
template <class TPlatform>
std::vector<keyboard::scan_code> glfw<TPlatform>::physical_keys_ = {};
} // namespace jage::input::adapters