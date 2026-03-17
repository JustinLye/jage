#pragma once

#include <jage/engine/input/contexts/glfw.hpp>
#include <jage/engine/time/durations.hpp>
#include <jage/interop/glfw_glad.hpp>

#include <jage/engine/time/internal/concepts/real_number_duration.hpp>

namespace jage::engine::input::platforms {
template <class TContext> struct glfw {
  using context_type = TContext;
  using window_handle_pointer_type = GLFWwindow *;
  using duration_type = time::durations::seconds;
  using user_pointer_type = void *;
  using monitor_pointer_type = GLFWmonitor *;
  using video_mode_pointer_type = const GLFWvidmode *;

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

  [[nodiscard]] static auto get_primary_monitor() -> monitor_pointer_type {
    return glfwGetPrimaryMonitor();
  }

  [[nodiscard]] static auto
  get_video_mode(monitor_pointer_type monitor) -> video_mode_pointer_type {
    return glfwGetVideoMode(monitor);
  }

  static auto set_window_hint(int hint, int value) -> void {
    glfwWindowHint(hint, value);
  }

  [[nodiscard]] static auto create_window(
      int width, int height, std::string_view title,
      monitor_pointer_type monitor,
      window_handle_pointer_type shared_window) -> window_handle_pointer_type {
    return glfwCreateWindow(width, height, std::data(title), monitor,
                            shared_window);
  }

  static auto set_current_context(window_handle_pointer_type window) -> void {
    glfwMakeContextCurrent(window);
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

  static auto set_swap_interval(const auto interval) -> void {
    glfwSwapInterval(interval);
  }

  struct content_scale {
    float x{};
    float y{};
  };

  [[nodiscard]] static auto
  get_content_scale(window_handle_pointer_type window) -> content_scale {
    auto scale = content_scale{};
    glfwGetWindowContentScale(window, &scale.x, &scale.y);
    return scale;
  }

  struct framebuffer_size {
    int width{};
    int height{};
  };

  [[nodiscard]] static auto
  get_framebuffer_size(window_handle_pointer_type window) -> framebuffer_size {
    auto size = framebuffer_size{};
    glfwGetFramebufferSize(window, &size.width, &size.height);
    return size;
  }

  [[nodiscard]] static auto
  window_should_close(window_handle_pointer_type window) -> bool {
    return glfwWindowShouldClose(window);
  }

  static auto poll_events() -> void { glfwPollEvents(); }

  static auto
  set_window_should_close(window_handle_pointer_type window) -> void {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
};
} // namespace jage::engine::input::platforms