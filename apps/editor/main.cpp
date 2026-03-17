#include <jage/engine/containers/spmc/ring_buffer.hpp>
#include <jage/engine/input/adapters/glfw.hpp>
#include <jage/engine/input/contexts/glfw.hpp>
#include <jage/engine/input/event.hpp>
#include <jage/engine/input/event_formatters.hpp>
#include <jage/engine/input/platforms/glfw.hpp>
#include <jage/engine/time/clock.hpp>
#include <jage/engine/time/durations.hpp>
#include <jage/engine/time/events/snapshot.hpp>
#include <jage/engine/time/hertz.hpp>
#include <jage/interop/glfw_glad.hpp>
#include <jage/stdx/overloaded.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdint>
#include <fmt/format.h>
#include <print>
#include <string>
#include <variant>
#include <vector>

using duration_type = jage::engine::time::durations::nanoseconds;
using event_type = jage::engine::input::event<duration_type>;

auto draw_frame_stats_panel(
    const jage::engine::time::events::snapshot<duration_type> &snap) -> void {
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

class event_log_panel {
  std::size_t next_write_index_{};
  std::array<std::optional<std::string>, 500> event_buffer_{};

public:
  auto push_back(const auto event) -> void {
    event_buffer_[next_write_index_++ % 500] = fmt::format("{}", event);
  }

  auto draw() -> void {
    ImGui::Begin("Input Events");
    ImGui::Text("Events: %zu", next_write_index_);
    ImGui::Separator();

    if (ImGui::BeginChild("EventScroll", ImVec2(0, 0), ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar)) {
      const auto beginning_index =
          next_write_index_ < 500 ? 0 : next_write_index_;
      for (auto offset = 0UZ; offset < 500; ++offset) {
        auto index = (beginning_index + offset) % 500;
        const auto &entry = event_buffer_[index];

        if (not entry.and_then(
                [&](const auto &text) -> std::optional<std::string> {
                  auto line = fmt::format("[{}]: {}", index, text);
                  ImGui::TextUnformatted(line.c_str());
                  return text;
                })) [[unlikely]] {
          break;
        }
      }
      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0F);
      }
    }
    ImGui::EndChild();
    ImGui::End();
  }
};

static constexpr auto process_input_events = [](auto &read_index,
                                                const auto &event_buffer,
                                                auto &platform, auto window,
                                                auto &event_display_panel) {
  const auto write_index = event_buffer.write_head();
  while (read_index < write_index) {
    auto next_event = event_buffer.read(read_index % event_buffer.capacity());
    ++read_index;
    event_display_panel.push_back(next_event);

    std::visit(jage::stdx::overloaded{
                   [&](const jage::engine::input::keyboard::events::key_press
                           &key_press) -> void {
                     if (key_press.scancode ==
                         jage::engine::input::keyboard::scancode::escape) {
                       platform.set_window_should_close(window);
                     }
                   },
                   [](const auto &) -> void {},
               },
               next_event.payload);
  }
};

auto main(int, char *[]) -> int {
  static constexpr auto frame_buffer_size_callback =
      [](GLFWwindow *, auto width, auto height) -> void {
    glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
  };

  using buffer_type =
      jage::engine::containers::spmc::ring_buffer<event_type, 256>;
  auto event_buffer = buffer_type{};

  using context_type =
      jage::engine::input::contexts::glfw<duration_type, buffer_type>;
  using platform_type = jage::engine::input::platforms::glfw<context_type>;
  auto context = context_type{event_buffer};
  auto platform = platform_type{};

  std::ignore = platform.initialize();

  platform.set_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  platform.set_window_hint(GLFW_CONTEXT_VERSION_MINOR, 3);
  platform.set_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  const auto monitor = platform.get_primary_monitor();
  const auto mode = platform.get_video_mode(monitor);

  platform.set_window_hint(GLFW_RED_BITS, mode->redBits);
  platform.set_window_hint(GLFW_GREEN_BITS, mode->greenBits);
  platform.set_window_hint(GLFW_BLUE_BITS, mode->blueBits);
  platform.set_window_hint(GLFW_REFRESH_RATE, mode->refreshRate);

  auto window = platform.create_window(mode->width, mode->height, "JAGE Editor",
                                       monitor, nullptr);

  platform.set_current_context(window);
  platform.set_swap_interval(1);

  std::ignore =
      gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  platform.set_window_user_pointer(window, static_cast<void *>(&context));

  auto adapter = jage::engine::input::adapters::glfw<platform_type>{};
  using jage::engine::time::operator""_Hz;
  auto clock = jage::engine::time::clock<duration_type>{60_Hz};
  adapter.initialize(window, platform, clock.real_time());

  platform.set_framebuffer_size_callback(window, frame_buffer_size_callback);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  auto &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  const auto &[x_scale, y_scale] = platform.get_content_scale(window);
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

  auto read_index = 0UZ;
  auto input_events_display_panel = event_log_panel{};

  while (not platform.window_should_close(window)) {
    platform.poll_events();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    draw_frame_stats_panel(clock.snapshot());
    process_input_events(read_index, event_buffer, platform, window,
                         input_events_display_panel);
    input_events_display_panel.draw();

    ImGui::Render();

    const auto &[width, height] = platform.get_framebuffer_size(window);
    glViewport(0, 0, width, height);
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
