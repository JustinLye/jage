#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

struct fake_ring_buffer {
  auto push(auto &&...) -> void { std::cout << "push called" << std::endl; }
};

struct input_system {
  fake_ring_buffer buffer_;
  input_system(GLFWwindow *window) {
    glfwSetWindowUserPointer(window, static_cast<void *>(&buffer_));
    glfwSetKeyCallback(window, key_callback);
  }
  static constexpr auto key_callback = [](GLFWwindow *window, int key, int,
                                          int action, int) -> void {
    static_cast<fake_ring_buffer *>(glfwGetWindowUserPointer(window))->push();
    if (GLFW_KEY_ESCAPE == key and GLFW_PRESS == action) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  };
};

auto main(int, char *[]) -> int {
  static constexpr auto frame_buffer_size_callback =
      [](GLFWwindow *, auto width, auto height) -> void {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
  };

  std::ignore = glfwInit();
  const auto monitor = glfwGetPrimaryMonitor();
  const auto mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  auto window = glfwCreateWindow(mode->width, mode->height, "Hello World",
                                 monitor, nullptr);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

  glfwMakeContextCurrent(window);
  std::ignore =
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
  [[maybe_unused]] auto in = input_system{window};

  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    glfwSwapBuffers(window);
  }
  return 0;
};