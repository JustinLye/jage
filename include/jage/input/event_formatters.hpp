#pragma once

#include <jage/input/event.hpp>
#include <jage/input/keyboard/action.hpp>
#include <jage/input/keyboard/events/key_press.hpp>
#include <jage/input/keyboard/key.hpp>
#include <jage/input/keyboard/scancode.hpp>
#include <jage/input/mouse/action.hpp>
#include <jage/input/mouse/button.hpp>
#include <jage/input/mouse/events/click.hpp>
#include <jage/input/mouse/events/cursor/motion.hpp>
#include <jage/input/mouse/events/cursor/position.hpp>
#include <jage/input/mouse/events/horizontal_scroll.hpp>
#include <jage/input/mouse/events/vertical_scroll.hpp>
#include <jage/time/durations.hpp>
#include <jage/time/events/snapshot.hpp>

#include <jage/ext/internal/overloaded.hpp>

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <string>
#include <string_view>

template <>
struct fmt::formatter<jage::input::keyboard::key>
    : fmt::formatter<std::string_view> {
  auto format(const jage::input::keyboard::key &input_key,
              fmt::format_context &ctx) const {
    return fmt::formatter<std::string_view>::format(
        jage::input::keyboard::serialize(input_key), ctx);
  }
};

template <>
struct fmt::formatter<jage::input::keyboard::action>
    : fmt::formatter<std::string_view> {
  auto format(const jage::input::keyboard::action &input_action,
              fmt::format_context &ctx) const {
    return fmt::formatter<std::string_view>::format(
        jage::input::keyboard::serialize(input_action), ctx);
  }
};

template <>
struct fmt::formatter<jage::input::keyboard::scancode>
    : fmt::formatter<std::string_view> {
  auto format(const jage::input::keyboard::scancode &input_scancode,
              fmt::format_context &ctx) const {
    return fmt::formatter<std::string_view>::format(
        jage::input::keyboard::serialize(input_scancode), ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::button>
    : fmt::formatter<std::string_view> {
  auto format(const jage::input::mouse::button &input_button,
              fmt::format_context &ctx) const {
    return fmt::formatter<std::string_view>::format(
        jage::input::mouse::serialize(input_button), ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::action>
    : fmt::formatter<std::string_view> {
  auto format(const jage::input::mouse::action &input_action,
              fmt::format_context &ctx) const {
    return fmt::formatter<std::string_view>::format(
        jage::input::mouse::serialize(input_action), ctx);
  }
};

template <>
struct fmt::formatter<jage::time::durations::nanoseconds>
    : fmt::formatter<std::string> {
  auto format(const jage::time::durations::nanoseconds &input_timestamp,
              fmt::format_context &ctx) const {
    auto timepoint = std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            input_timestamp)};
    return fmt::formatter<std::string>::format(
        fmt::format("{:%Y-%m-%d %H:%M:%S} ({} ns since epoch)", timepoint,
                    static_cast<std::uint64_t>(input_timestamp.count())),
        ctx);
  }
};

template <>
struct fmt::formatter<std::chrono::nanoseconds> : fmt::formatter<std::string> {
  auto format(const std::chrono::nanoseconds &input_timestamp,
              fmt::format_context &ctx) const {
    constexpr auto cst_adj =
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            std::chrono::nanoseconds{21600000000000});
    auto timepoint = std::chrono::system_clock::time_point{
        std::chrono::duration_cast<std::chrono::system_clock::duration>(
            input_timestamp) -
        cst_adj};
    return fmt::formatter<std::string>::format(
        fmt::format("{:%Y-%m-%d %H:%M:%S} ({} ns since epoch)", timepoint,
                    static_cast<std::uint64_t>(input_timestamp.count())),
        ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::events::click>
    : fmt::formatter<std::string> {
  auto format(const jage::input::mouse::events::click &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("mouse-click": {{
      "button": {},
      "action": {},
      "modifiers": {}
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.button,
                    input_event.action, input_event.modifiers.to_string()),
        ctx);
  }
};

template <>
struct fmt::formatter<jage::input::keyboard::events::key_press>
    : fmt::formatter<std::string> {
  auto format(const jage::input::keyboard::events::key_press &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("key-press": {{
      "key": {},
      "scancode": {},
      "action": {},
      "modifiers": {}
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.key,
                    input_event.scancode, input_event.action,
                    input_event.modifiers.to_string()),
        ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::events::cursor::position>
    : fmt::formatter<std::string> {
  auto format(const jage::input::mouse::events::cursor::position &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("cursor-position": {{
      "x": {},
      "y": {},
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.x, input_event.y),
        ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::events::cursor::motion>
    : fmt::formatter<std::string> {
  auto format(const jage::input::mouse::events::cursor::motion &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("cursor-motion": {{
      "delta_x": {},
      "delta_y": {}
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.delta_x,
                    input_event.delta_y),
        ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::events::horizontal_scroll>
    : fmt::formatter<std::string> {
  auto format(const jage::input::mouse::events::horizontal_scroll &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("horizontal-scroll": {{
      "offset": {}
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.offset), ctx);
  }
};

template <>
struct fmt::formatter<jage::input::mouse::events::vertical_scroll>
    : fmt::formatter<std::string> {
  auto format(const jage::input::mouse::events::vertical_scroll &input_event,
              fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("vertical-scroll": {{
      "offset": {}
  }})";
    return fmt::formatter<std::string>::format(
        fmt::format(fmt::runtime(template_text), input_event.offset), ctx);
  }
};

template <>
struct fmt::formatter<
    jage::time::events::snapshot<jage::time::durations::nanoseconds>>
    : fmt::formatter<std::string> {
  auto
  format(const jage::time::events::snapshot<jage::time::durations::nanoseconds>
             &input_event,
         fmt::format_context &ctx) const {
    static constexpr auto template_text = R"(
"frame": {{
  "wall-time": {},
  "snapshot": {{
    "real_time": {},
    "tick_duration": {},
    "time_scale": {},
    "elapsed_time": {},
    "elapsed_frames": {},
    "frame": {},
    "accumulated_time": {}
}})";
    return fmt::formatter<std::string>::format(
        fmt::format(
            fmt::runtime(template_text),
            std::chrono::high_resolution_clock::now().time_since_epoch(),
            input_event.real_time, input_event.tick_duration,
            input_event.time_scale, input_event.elapsed_time,
            input_event.elapsed_frames, input_event.frame,
            input_event.accumulated_time),
        ctx);
  }
};

template <>
struct fmt::formatter<
    jage::input::event<jage::time::durations::nanoseconds>::payload_type>
    : fmt::formatter<std::string> {
  auto format(
      const jage::input::event<jage::time::durations::nanoseconds>::payload_type
          &input_event,
      fmt::format_context &ctx) const {
    return fmt::formatter<std::string>::format(
        std::visit(jage::ext::internal::overloaded{
                       [&]<class T>(const T &payload) -> std::string
                         requires fmt::is_formattable<T>::value
                       { return fmt::format("{}", payload); },
                       [](auto &&) -> std::string {
                         return "event type formatter is missing";
                       },
                       },
                       input_event),
                   ctx);
  }
};

template <>
struct fmt::formatter<jage::input::event<jage::time::durations::nanoseconds>>
    : fmt::formatter<std::string> {
  auto format(
      const jage::input::event<jage::time::durations::nanoseconds> &input_event,
      fmt::format_context &ctx) const {
    static constexpr auto template_text = R"("input-event": {{
    "wall-time": {},
    "timestamp": {},
    {}
}})";
    return fmt::formatter<std::string>::format(
        fmt::format(
            fmt::runtime(template_text),
            std::chrono::high_resolution_clock::now().time_since_epoch(),
            input_event.timestamp, input_event.payload),
        ctx);
  }
};
