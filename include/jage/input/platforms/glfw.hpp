#pragma once

#include <jage/external/glfw.hpp>
#include <jage/input/contexts/glfw.hpp>
#include <jage/time/durations.hpp>

#include <jage/time/internal/concepts/real_number_duration.hpp>

namespace jage::input::platforms {
template <class TContext> struct glfw {
  using context_type = TContext;
  using window_handle_pointer_type = GLFWwindow *;
  using duration_type = time::durations::seconds;
  using user_pointer_type = void *;

  static auto set_window_user_pointer(window_handle_pointer_type window,
                                      user_pointer_type user_pointer) -> void {
    glfwSetWindowUserPointer(window, user_pointer);
  }

  [[nodiscard]] static auto get_window_user_pointer(
      window_handle_pointer_type window) -> user_pointer_type {
    return glfwGetWindowUserPointer(window);
  }

  static auto set_key_callback(window_handle_pointer_type window,
                               GLFWkeyfun callback) -> void {
    glfwSetKeyCallback(window, callback);
  }

  static auto set_mouse_button_callback(window_handle_pointer_type window,
                                        GLFWmousebuttonfun callback) -> void {
    glfwSetMouseButtonCallback(window, callback);
  }

  static auto set_cursor_position_callback(window_handle_pointer_type window,
                                           GLFWcursorposfun callback) -> void {
    glfwSetCursorPosCallback(window, callback);
  }

  static auto set_scroll_callback(window_handle_pointer_type window,
                                  GLFWscrollfun callback) -> void {
    glfwSetScrollCallback(window, callback);
  }

  static auto set_input_mode(window_handle_pointer_type window, int mode,
                             int value) -> void {
    glfwSetInputMode(window, mode, value);
  }

  [[nodiscard]] static auto get_input_mode(window_handle_pointer_type window,
                                           int mode) -> int {
    return glfwGetInputMode(window, mode);
  }

  [[nodiscard]] static auto get_key_scancode(int key) -> int {
    return glfwGetKeyScancode(key);
  }

  [[nodiscard]] static auto get_seconds_since_init() -> duration_type {
    return duration_type{glfwGetTime()};
  }

  static auto set_seconds_since_init(const duration_type seconds) -> void {
    glfwSetTime(seconds.count());
  }

  [[nodiscard]] static auto initialize() -> int { return glfwInit(); }

  [[nodiscard]] static auto create_window() -> window_handle_pointer_type {
    const auto monitor = glfwGetPrimaryMonitor();
    const auto mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    auto window = glfwCreateWindow(mode->width, mode->height, "Hello World",
                                   monitor, nullptr);
    glfwMakeContextCurrent(window);
    return window;
  }

  static auto
  set_framebuffer_size_callback(window_handle_pointer_type window,
                                GLFWframebuffersizefun callback) -> void {
    glfwSetFramebufferSizeCallback(window, callback);
  }

  [[nodiscard]] static auto refresh_rate() -> int {
    const auto monitor = glfwGetPrimaryMonitor();
    const auto mode = glfwGetVideoMode(monitor);
    return mode->refreshRate;
  }
};
} // namespace jage::input::platforms