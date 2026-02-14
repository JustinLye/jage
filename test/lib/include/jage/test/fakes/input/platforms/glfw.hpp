#pragma once

#include <jage/time/durations.hpp>

#include <jage/test/fakes/input/context/glfw.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>

namespace jage::test::fakes::input::platforms {

// TODO: Make this a concept
template <class TContext> class glfw {
  using user_pointer_type_ = void *;
  using window_handle_type_ = std::uint8_t;
  using window_handle_pointer_type_ = window_handle_type_ *;
  using key_callback_type_ = void (*)(window_handle_pointer_type_, int, int,
                                      int, int);
  using context_type_ = TContext;

  window_handle_type_ window_handle_{0};
  user_pointer_type_ user_pointer_{nullptr};
  key_callback_type_ key_callback_{nullptr};
  time::seconds seconds_since_init_{0};
  bool initialized_{false};

  static std::unique_ptr<glfw> instance_;

  auto trigger_callback(auto callback, auto &&...args) -> void {
    if (nullptr != callback) {
      callback(&window_handle_, std::forward<decltype(args)>(args)...);
    }
  }

  [[nodiscard]] static auto get_instance() -> glfw & {
    if (nullptr == instance_) {
      instance_ = std::make_unique<glfw>();
    }
    return *instance_.get();
  }

public:
  using window_handler_pointer_type = window_handle_pointer_type_;
  using user_pointer_type = user_pointer_type_;
  using key_callback_type = key_callback_type_;
  using context_type = context_type_;
  using duration_type = time::seconds;

  static std::unordered_map<int, int> key_to_scancode;

  static auto set_window_user_pointer(window_handler_pointer_type,
                                      user_pointer_type user_pointer) -> void {
    get_instance().user_pointer_ = user_pointer;
  }

  [[nodiscard]] static auto
  get_window_user_pointer(window_handler_pointer_type) -> user_pointer_type {
    return get_instance().user_pointer_;
  }

  static auto set_key_callback(window_handler_pointer_type,
                               key_callback_type key_callback) -> void {
    get_instance().key_callback_ = key_callback;
  }

  static auto trigger_key_callback(int key, int scancode, int action,
                                   int mods) -> void {
    auto &instance = get_instance();
    instance.trigger_callback(instance.key_callback_, key, scancode, action,
                              mods);
  }

  [[nodiscard]] static auto get_key_scancode(int key) -> int {
    if (const auto iter = key_to_scancode.find(key);
        iter != std::end(key_to_scancode)) {
      return iter->second;
    }
    return -1;
  }

  [[nodiscard]] static auto get_seconds_since_init() -> duration_type {
    return get_instance().seconds_since_init_;
  }

  static auto set_seconds_since_init(const duration_type seconds) -> void {
    get_instance().seconds_since_init_ = seconds;
  }

  static auto initialize() -> void { get_instance().initialized_ = true; }

  [[nodiscard]] auto is_initialized() const -> bool {
    return get_instance().initialized_;
  }

  auto reset() -> void { instance_.reset(); }
};

template <class T> std::unique_ptr<glfw<T>> glfw<T>::instance_ = nullptr;
template <class T> std::unordered_map<int, int> glfw<T>::key_to_scancode = {};

} // namespace jage::test::fakes::input::platforms