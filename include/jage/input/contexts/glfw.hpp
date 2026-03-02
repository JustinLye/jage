#pragma once

#include <jage/input/event.hpp>
#include <jage/time/durations.hpp>

#include <jage/input/internal/concepts/event_sink.hpp>
#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <functional>
#include <utility>

namespace jage::input::contexts {
template <time::internal::concepts::real_number_duration TTimeDuration,
          class TEventSink>
  requires internal::concepts::event_sink<TEventSink, event<TTimeDuration>>
class glfw {
  using duration_type_ = TTimeDuration;
  using event_type_ = event<duration_type_>;
  using cursor_position_type_ = std::pair<double, double>;

  cursor_position_type_ cursor_position_{};
  std::reference_wrapper<TEventSink> event_sink_{};

public:
  using duration_type = duration_type_;
  using event_type = event_type_;

  explicit glfw(TEventSink &event_sink) : event_sink_{event_sink} {}

  auto push(event_type &&event) -> void {
    event_sink_.get().push(std::move(event));
  }

  auto push(const event_type &event) -> void { event_sink_.get().push(event); }

  [[nodiscard]] auto
  last_known_cursor_position() const -> const cursor_position_type_ & {
    return cursor_position_;
  }

  [[nodiscard]] auto last_known_cursor_position() -> cursor_position_type_ & {
    return cursor_position_;
  }
};

} // namespace jage::input::contexts