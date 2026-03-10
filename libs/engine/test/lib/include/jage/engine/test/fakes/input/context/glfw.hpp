#pragma once

#include <jage/engine/input/internal/event.hpp>
#include <jage/engine/time/internal/concepts/real_number_duration.hpp>

#include <deque>
#include <utility>

namespace jage::engine::test::fakes::input::context {
template <class...> struct glfw {};

template <
    ::jage::engine::time::internal::concepts::real_number_duration TDuration,
    class... TPayloads>
struct glfw<TDuration,
            ::jage::engine::input::internal::event<TDuration, TPayloads...>> {
  using event_type =
      ::jage::engine::input::internal::event<TDuration, TPayloads...>;
  using duration_type = TDuration;
  std::deque<event_type> buffer;
  std::pair<double, double> last_known_cursor_position_{};

  [[nodiscard]] auto
  last_known_cursor_position() const -> const std::pair<double, double> & {
    return last_known_cursor_position_;
  }
  [[nodiscard]] auto
  last_known_cursor_position() -> std::pair<double, double> & {
    return last_known_cursor_position_;
  }
  auto push(auto &&event) -> void {
    buffer.push_back(std::forward<decltype(event)>(event));
  }
};
} // namespace jage::engine::test::fakes::input::context
