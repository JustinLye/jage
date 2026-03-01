#include <jage/containers/spmc/ring_buffer.hpp>
#include <jage/external/glfw.hpp>
#include <jage/input/adapters/glfw.hpp>
#include <jage/input/contexts/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/event_formatters.hpp>
#include <jage/input/platforms/glfw.hpp>
#include <jage/scheduled_action.hpp>
#include <jage/time/clock.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>
#include <jage/time/hertz.hpp>

#include <jage/ext/internal/overloaded.hpp>

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <iostream>
#include <string>
#include <variant>

using duration_type = jage::time::durations::nanoseconds;

auto operator<<(std::ostream &out,
                const typename jage::input::event<duration_type> &input_event)
    -> std::ostream & {
  out << fmt::format("{}\n", input_event);
  return out;
}

auto operator<<(std::ostream &out,
                const jage::time::events::snapshot<duration_type> &snapshot)
    -> std::ostream & {
  out << fmt::format("{}", snapshot) << std::endl;
  return out;
}

auto main(int, char *[]) -> int {
  static constexpr auto frame_buffer_size_callback =
      [](GLFWwindow *, auto width, auto height) -> void {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
  };

  using event_type = jage::input::event<duration_type>;
  using buffer_type = jage::containers::spmc::ring_buffer<event_type, 256>;
  auto event_buffer = buffer_type{};

  using context_type = jage::input::contexts::glfw<duration_type, buffer_type>;
  using platform_type = jage::input::platforms::glfw<context_type>;
  auto context = context_type{event_buffer};
  auto platform = platform_type{};

  std::ignore = platform.initialize();
  auto window = platform.create_window();
  platform.set_window_user_pointer(window, static_cast<void *>(&context));

  auto adapter = jage::input::adapters::glfw<platform_type>{};
  using jage::time::operator""_Hz;
  auto refresh_rate = platform.refresh_rate();
  auto clock =
      jage::time::clock<duration_type>{jage::time::hertz{refresh_rate}};
  adapter.initialize(window, platform, clock.real_time());

  std::ignore =
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  platform.set_framebuffer_size_callback(window, frame_buffer_size_callback);

  platform.set_input_mode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
  auto read_index = 0UZ;

  using namespace std::chrono_literals;

  auto last_snapshot = clock.snapshot();
  auto loop_count = 0UZ;
  auto sample_count = 0UZ;
  auto average_fps = 0.0;
  auto average_loop_count = 0.0;
  auto average_event_count = 0.0;
  auto max_event_count = 0.0;
  auto output_snapshot = jage::scheduled_action{
      1s, [&] {
        const auto current_snapshot = clock.snapshot();
        ++sample_count;
        const auto current_fps = current_snapshot.frame - last_snapshot.frame;
        average_fps +=
            (static_cast<double>(current_fps) - average_fps) / sample_count;
        average_loop_count +=
            (static_cast<double>(loop_count) - average_loop_count) /
            sample_count;
        std::cout << "FPS: " << current_fps << '\n'
                  << "Loop counter: " << loop_count << '\n'
                  << "Avg. FPS: " << average_fps << '\n'
                  << "Avg. Loop: " << average_loop_count << '\n'
                  << "Event Count: " << average_event_count << '\n'
                  << "Max Event Count: " << max_event_count << '\n'
                  << current_snapshot << std::endl;
        last_snapshot = current_snapshot;
        loop_count = 0UZ;
        max_event_count = std::max(max_event_count, average_event_count);
        average_event_count = 0.0;
      }};

  auto last_real_time = clock.real_time();
  auto swap_interval = 1;
  while (!glfwWindowShouldClose(window)) {
    auto current_real_time = clock.real_time();
    output_snapshot.update(std::chrono::duration_cast<std::chrono::nanoseconds>(
        current_real_time - last_real_time));

    last_real_time = current_real_time;
    if (output_snapshot.is_complete()) [[unlikely]] {
      output_snapshot.reset(1s);
    }
    glClear(GL_COLOR_BUFFER_BIT);
    glfwPollEvents();
    const auto write_index = event_buffer.write_head();
    const auto event_count =
        static_cast<double>(write_index) - static_cast<double>(read_index);
    while (read_index < write_index) {
      auto next_input_event =
          event_buffer.read(read_index % event_buffer.capacity());
      ++read_index;
      std::cout << next_input_event;
      std::visit(
          jage::ext::internal::overloaded{
              [](auto &&) -> void {},
              [&](jage::input::keyboard::events::key_press key_press) -> void {
                switch (key_press.scancode) {
                case jage::input::keyboard::scancode::escape:
                  glfwSetWindowShouldClose(window, GLFW_TRUE);
                  break;
                case jage::input::keyboard::scancode::l: {
                  if (jage::input::keyboard::action::release !=
                      key_press.action) {
                    break;
                  }
                  if (GLFW_CURSOR_DISABLED ==
                      platform.get_input_mode(window, GLFW_CURSOR)) {
                    std::cout << "enabling cursor\n";
                    platform.set_input_mode(window, GLFW_CURSOR,
                                            GLFW_CURSOR_NORMAL);
                  } else {
                    std::cout << "disabling cursor\n";
                    platform.set_input_mode(window, GLFW_CURSOR,
                                            GLFW_CURSOR_DISABLED);
                  }
                } break;
                case jage::input::keyboard::scancode::p: {
                  if (jage::input::keyboard::action::release !=
                      key_press.action) {
                    break;
                  }
                  const auto snapshot = clock.snapshot();
                  const auto new_time_scale = 1.0 - 1.0 * snapshot.time_scale;
                  std::cout << "setting time scale to " << new_time_scale
                            << std::endl;
                  clock.set_time_scale(new_time_scale);
                  if (new_time_scale == 0) {
                    output_snapshot.pause();
                  } else {
                    output_snapshot.resume();
                  }
                } break;
                case jage::input::keyboard::scancode::v: {
                  if (jage::input::keyboard::action::release !=
                      key_press.action) {
                    break;
                  }
                  if (swap_interval) {
                    swap_interval = 0;
                  } else {
                    swap_interval = 1;
                  }
                  glfwSwapInterval(swap_interval);
                } break;
                default:
                  break;
                }
              },
          },
          next_input_event.payload);
    }
    glfwSwapBuffers(window);
    ++loop_count;
    if (event_count > 0) {
      average_event_count +=
          (event_count - average_event_count) / static_cast<double>(loop_count);
    }
  }
  return 0;
};