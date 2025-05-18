#pragma once

#include <jage/no_op.hpp>
#include <jage/scheduled_action_status.hpp>

#include <chrono>
#include <utility>

namespace jage {
template <class TAction = no_op> class scheduled_action {
  scheduled_action_status scheduled_action_status_ =
      scheduled_action_status::active;
  std::chrono::nanoseconds nanoseconds_to_wait_{};
  TAction action_;

public:
  scheduled_action() = default;
  scheduled_action(const std::chrono::nanoseconds nanoseconds_to_wait)
      : nanoseconds_to_wait_(nanoseconds_to_wait) {}
  scheduled_action(const std::chrono::nanoseconds nanoseconds_to_wait,
                   TAction &&action)
      : nanoseconds_to_wait_(nanoseconds_to_wait), action_(action) {}

  [[gnu::pure, nodiscard]] auto
  status() const noexcept -> const scheduled_action_status & {
    return scheduled_action_status_;
  }

  auto pause() noexcept -> void {
    if (scheduled_action_status_ != scheduled_action_status::canceled) {
      scheduled_action_status_ = scheduled_action_status::paused;
    }
  }

  auto resume() noexcept -> void {
    if (scheduled_action_status_ != scheduled_action_status::canceled) {
      scheduled_action_status_ = scheduled_action_status::active;
    }
  }

  auto cancel() noexcept -> void {
    if (scheduled_action_status_ != scheduled_action_status::complete) {
      scheduled_action_status_ = scheduled_action_status::canceled;
    }
  }

  auto reset(const auto nanoseconds_to_wait) noexcept -> void {
    scheduled_action_status_ = scheduled_action_status::active;
    nanoseconds_to_wait_ = nanoseconds_to_wait;
  }

  auto extend(const auto additional_nanoseconds) noexcept -> void {
    nanoseconds_to_wait_ += additional_nanoseconds;
  }

  auto update(const auto nanoseconds_elapsed) {
    using namespace std::chrono_literals;
    if (is_complete()) {
      return;
    }
    if (nanoseconds_to_wait_ < nanoseconds_elapsed) {
      nanoseconds_to_wait_ = 0ns;
    } else {
      nanoseconds_to_wait_ -= nanoseconds_elapsed;
    }

    if (nanoseconds_to_wait_ == 0ns) {
      scheduled_action_status_ = scheduled_action_status::complete;
      action_();
    }
  }

  [[gnu::pure, nodiscard]] auto is_complete() const noexcept -> bool {
    switch (status()) {
    case scheduled_action_status::complete:
    case scheduled_action_status::canceled:
      return true;
    case scheduled_action_status::paused:
    case scheduled_action_status::active:
      return false;
    }
    std::unreachable();
  }
};

template <class TAction>
scheduled_action(const std::chrono::nanoseconds,
                 TAction &&) -> scheduled_action<TAction>;

} // namespace jage