#include <jage/containers/spmc/ring_buffer.hpp>
#include <jage/external/glfw.hpp>
#include <jage/input/adapters/glfw.hpp>
#include <jage/input/contexts/glfw.hpp>
#include <jage/input/event.hpp>
#include <jage/input/platforms/glfw.hpp>
#include <jage/time/clock.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>
#include <jage/time/hertz.hpp>

#include <jage/ext/internal/overloaded.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdint>
#include <fmt/format.h>
#include <print>
#include <string>
#include <variant>
#include <vector>

using duration_type = jage::time::durations::nanoseconds;
using event_type = jage::input::event<duration_type>;

struct event_log_entry {
  std::string text;
};

auto format_keyboard_event(
    const jage::input::keyboard::events::key_press &ev) -> std::string {
  return fmt::format(
      "[key] {} ({}) {} mods={}", jage::input::keyboard::serialize(ev.key),
      jage::input::keyboard::serialize(ev.scancode),
      jage::input::keyboard::serialize(ev.action), ev.modifiers.to_string());
}

auto format_mouse_click(
    const jage::input::mouse::events::click &ev) -> std::string {
  return fmt::format(
      "[mouse] {} {} mods={}", jage::input::mouse::serialize(ev.button),
      jage::input::mouse::serialize(ev.action), ev.modifiers.to_string());
}

auto format_cursor_position(
    const jage::input::mouse::events::cursor::position &ev) -> std::string {
  return fmt::format("[cursor] pos ({:.1f}, {:.1f})", ev.x, ev.y);
}

auto format_cursor_motion(
    const jage::input::mouse::events::cursor::motion &ev) -> std::string {
  return fmt::format("[cursor] delta ({:.1f}, {:.1f})", ev.delta_x, ev.delta_y);
}

auto format_horizontal_scroll(
    const jage::input::mouse::events::horizontal_scroll &ev) -> std::string {
  return fmt::format("[scroll] horizontal {:.2f}", ev.offset);
}

auto format_vertical_scroll(
    const jage::input::mouse::events::vertical_scroll &ev) -> std::string {
  return fmt::format("[scroll] vertical {:.2f}", ev.offset);
}

auto format_event(const event_type &ev) -> std::string {
  return std::visit(
      jage::ext::internal::overloaded{
          [](const jage::input::keyboard::events::key_press &e) {
            return format_keyboard_event(e);
          },
          [](const jage::input::mouse::events::click &e) {
            return format_mouse_click(e);
          },
          [](const jage::input::mouse::events::cursor::position &e) {
            return format_cursor_position(e);
          },
          [](const jage::input::mouse::events::cursor::motion &e) {
            return format_cursor_motion(e);
          },
          [](const jage::input::mouse::events::horizontal_scroll &e) {
            return format_horizontal_scroll(e);
          },
          [](const jage::input::mouse::events::vertical_scroll &e) {
            return format_vertical_scroll(e);
          },
      },
      ev.payload);
}

auto draw_frame_stats_panel(
    const jage::time::events::snapshot<duration_type> &snap) -> void {
  ImGui::Begin("Frame Stats");
  ImGui::Text("Frame: %lu", snap.frame);
  ImGui::Text("Time Scale: %.2f", snap.time_scale);
  ImGui::Text("Tick Duration: %.3f ms",
              snap.tick_duration.count() / 1'000'000.0);
  ImGui::Text("Elapsed Frames: %lu", snap.elapsed_frames);
  ImGui::Text("Accumulated Time: %.3f ms",
              snap.accumulated_time.count() / 1'000'000.0);
  ImGui::End();
}

auto draw_event_log_panel(const std::vector<event_log_entry> &log) -> void {
  ImGui::Begin("Input Events");
  ImGui::Text("Events: %zu", log.size());
  ImGui::Separator();

  if (ImGui::BeginChild("EventScroll", ImVec2(0, 0), ImGuiChildFlags_None,
                        ImGuiWindowFlags_HorizontalScrollbar)) {
    for (const auto &entry : log) {
      ImGui::TextUnformatted(entry.text.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(1.0F);
    }
  }
  ImGui::EndChild();
  ImGui::End();
}

auto main(int, char *[]) -> int {
  static constexpr auto frame_buffer_size_callback =
      [](GLFWwindow *, auto width, auto height) -> void {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
  };

  using buffer_type = jage::containers::spmc::ring_buffer<event_type, 256>;
  auto event_buffer = buffer_type{};

  using context_type = jage::input::contexts::glfw<duration_type, buffer_type>;
  using platform_type = jage::input::platforms::glfw<context_type>;
  auto context = context_type{event_buffer};
  auto platform = platform_type{};

  std::ignore = platform.initialize();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const auto monitor = glfwGetPrimaryMonitor();
  const auto mode = glfwGetVideoMode(monitor);

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  auto window = glfwCreateWindow(mode->width, mode->height, "JAGE Editor",
                                 monitor, nullptr);

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  std::ignore =
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  platform.set_window_user_pointer(window, static_cast<void *>(&context));

  auto adapter = jage::input::adapters::glfw<platform_type>{};
  using jage::time::operator""_Hz;
  auto clock = jage::time::clock<duration_type>{60_Hz};
  adapter.initialize(window, platform, clock.real_time());

  platform.set_framebuffer_size_callback(window, frame_buffer_size_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  float x_scale = 1.0F;
  float y_scale = 1.0F;
  glfwGetWindowContentScale(window, &x_scale, &y_scale);
  float dpi_scale = std::max(x_scale, y_scale);
  std::println("dpi_scale: {}", dpi_scale);

  ImGui::StyleColorsDark();
  ImGui::GetStyle().ScaleAllSizes(dpi_scale);

  ImFontConfig font_config;
  constexpr auto font_size = 13.0F;
  font_config.SizePixels = font_size * dpi_scale;
  io.Fonts->AddFontDefault(&font_config);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  auto event_log = std::vector<event_log_entry>{};
  static constexpr auto max_log_entries = 500UZ;

  auto read_index = 0UZ;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    const auto snap = clock.snapshot();
    const auto write_index = event_buffer.write_head();

    while (read_index < write_index) {
      auto next_event = event_buffer.read(read_index % event_buffer.capacity());
      ++read_index;

      std::visit(
          jage::ext::internal::overloaded{
              [&](const jage::input::keyboard::events::key_press &key_press)
                  -> void {
                if (key_press.scancode ==
                    jage::input::keyboard::scancode::escape) {
                  glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
              },
              [](const auto &) -> void {},
          },
          next_event.payload);

      event_log.push_back({.text = format_event(next_event)});
      if (event_log.size() > max_log_entries) {
        event_log.erase(event_log.begin());
      }
    }

    draw_frame_stats_panel(snap);
    draw_event_log_panel(event_log);

    ImGui::Render();
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
