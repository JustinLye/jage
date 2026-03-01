#pragma once

#include <deque>
#include <utility>

namespace jage::test::fakes::containers {
template <class TEvent> struct event_sink {
  std::deque<TEvent> events;

  auto push(TEvent &&event) -> void { events.push_back(std::move(event)); }

  auto push(const TEvent &event) -> void { events.push_back(event); }
};
} // namespace jage::test::fakes::containers
