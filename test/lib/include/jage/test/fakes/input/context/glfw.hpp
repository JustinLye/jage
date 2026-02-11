#pragma once

#include <jage/time/internal/concepts/real_number_duration.hpp>

#include <deque>
#include <utility>
#include <variant>

namespace jage::test::fakes::input::context {
template <::jage::time::internal::concepts::real_number_duration TDuration,
          class... TEvents>
struct glfw {
  using event_type = std::variant<TEvents...>;
  using duration_type = TDuration;
  std::deque<event_type> buffer;

  auto push(auto &&event) -> void {
    buffer.push_back(std::forward<decltype(event)>(event));
  }
};
} // namespace jage::test::fakes::input::context