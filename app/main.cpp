#include <jage/game.hpp>
#include <jage/input/controller.hpp>
#include <jage/window.hpp>

// for driver
#include <jage/input/button/states.hpp>
#include <jage/input/cursor/state.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace jage {

class driver {

  GLFWwindow *window_{nullptr};
  static constexpr auto frame_buffer_size_callback(GLFWwindow *, auto width,
                                                   auto height) -> void {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
  }

public:
  driver() {
    std::ignore = glfwInit();
    const auto monitor = glfwGetPrimaryMonitor();
    const auto mode = glfwGetVideoMode(monitor);

    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    window_ = glfwCreateWindow(mode->width, mode->height, "Hello World",
                               monitor, nullptr);
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

    glfwMakeContextCurrent(window_);
    std::ignore =
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
    glfwSetFramebufferSizeCallback(window_, &frame_buffer_size_callback);
  }

  ~driver() { glfwTerminate(); }

  auto poll() -> void {
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  [[nodiscard]] auto is_down(const input::mouse::buttons button) const -> bool {
    return GLFW_PRESS ==
           glfwGetMouseButton(window_,
                              static_cast<int>(std::to_underlying(button)));
  }

  [[nodiscard]] auto is_down(const input::keyboard::key key) const -> bool {
    switch (key) {
    case input::keyboard::key::escape:
      return GLFW_PRESS == glfwGetKey(window_, GLFW_KEY_ESCAPE);
    default:
      return false;
    }
  }

  [[nodiscard]] auto cursor_position() const -> input::cursor::state {
    return {};
  }

  [[nodiscard]] auto should_close() const -> bool {
    return glfwWindowShouldClose(window_);
  }

  auto close() -> void { glfwSetWindowShouldClose(window_, GLFW_TRUE); }

  auto render() -> void { glClear(GL_COLOR_BUFFER_BIT); }
};
} // namespace jage

auto main(int, char *[]) -> int {
  jage::driver driver{};

  jage::input::controller input_controller{driver};
  auto &keyboard = input_controller.keyboard();
  keyboard.monitor_input(jage::input::keyboard::key::escape);
  std::ignore = keyboard.register_callback(
      [&](const jage::input::button::states<jage::input::keyboard::key> states)
          -> void {
        if (jage::input::button::status::down ==
            states[jage::input::keyboard::key::escape].status) {
          driver.close();
        }
      });
  jage::window window{driver};
  jage::game game{window, input_controller};

  game.loop();

  return 0;
};