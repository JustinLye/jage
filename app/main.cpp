#include <jage/game.hpp>
#include <jage/input/controller.hpp>
#include <jage/window.hpp>

// for driver
#include <jage/input/button/states.hpp>
#include <jage/input/cursor/state.hpp>
#include <jage/input/keyboard/keys.hpp>
#include <jage/input/mouse/buttons.hpp>

#include <GLFW/glfw3.h>

// debug
#include <iostream>

namespace jage {
class driver {

  GLFWwindow *window_{nullptr};

public:
  driver() {
    std::ignore = glfwInit();
    window_ = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    glfwMakeContextCurrent(window_);
    glfwSetInputMode(window_, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window_, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
  }

  ~driver() { glfwTerminate(); }

  auto poll() -> void {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window_);
    glfwPollEvents();
  }

  [[nodiscard]] auto is_down(const input::mouse::buttons button) const -> bool {
    return GLFW_PRESS ==
           glfwGetMouseButton(window_,
                              static_cast<int>(std::to_underlying(button)));
  }

  [[nodiscard]] auto is_down(const input::keyboard::keys key) const -> bool {
    switch (key) {
    case input::keyboard::keys::escape:
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

  auto close() -> void {
    std::cerr << "setting close" << std::endl;
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
  }
};
} // namespace jage

auto main(int, char *[]) -> int {
  jage::driver driver{};

  jage::input::controller input_controller{driver};
  auto &keyboard = input_controller.keyboard();
  keyboard.monitor_input(jage::input::keyboard::keys::escape);
  std::ignore = keyboard.register_callback(
      [&](const jage::input::button::states<jage::input::keyboard::keys> states)
          -> void {
        if (jage::input::button::status::down ==
            states[jage::input::keyboard::keys::escape].status) {
          driver.close();
        }
      });
  jage::window window{driver};
  jage::game game{window, input_controller};

  game.loop();

  return 0;
};