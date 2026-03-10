#pragma once

#include <jage/input/internal/event.hpp>
#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <deque>
#include <utility>

namespace jage::test::fakes::input::context {
template <class...> struct glfw {};

template <::jage::time::internal::concepts::real_number_duration TDuration,
          class... TPayloads>
struct glfw<TDuration,
            ::jage::input::internal::event<TDuration, TPayloads...>> {
  using event_type = ::jage::input::internal::event<TDuration, TPayloads...>;
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
} // namespace jage::test::fakes::input::context
