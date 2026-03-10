#pragma once

#include <concepts>
#include <utility>

namespace jage::input::internal::concepts {
template <class TBuffer, class TEvent>
concept event_sink = requires(TBuffer buffer, TEvent event) {
  { buffer.push(std::move(event)) } -> std::same_as<void>;
  { buffer.push(std::as_const(event)) } -> std::same_as<void>;
};
} // namespace jage::input::internal::concepts
