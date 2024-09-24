#pragma once

#include <chrono>
#include <cstdint>
#include <utility>

namespace jage {
class scheduled_action {
  std::chrono::nanoseconds nanoseconds_to_wait_{};

public:
  enum class status : std::uint8_t {
    paused,
    active,
    canceled,
    complete,
  };

  scheduled_action() = default;
  scheduled_action(const auto nanoseconds_to_wait)
      : nanoseconds_to_wait_(nanoseconds_to_wait) {}

  [[gnu::pure, nodiscard]] auto status() const noexcept -> const status & {
    return status_;
  }

  auto pause() noexcept -> void {
    if (status_ != status::canceled) {
      status_ = status::paused;
    }
  }

  auto resume() noexcept -> void {
    if (status_ != status::canceled) {
      status_ = status::active;
    }
  }

  auto cancel() noexcept -> void {
    if (status_ != status::complete) {
      status_ = status::canceled;
    }
  }

  auto reset(const auto nanoseconds_to_wait) noexcept -> void {
    status_ = status::active;
    nanoseconds_to_wait_ = nanoseconds_to_wait;
  }

  auto extend(const auto additional_nanoseconds) noexcept -> void {
    nanoseconds_to_wait_ += additional_nanoseconds;
  }

  auto update(const auto nanoseconds_elapsed) {
    using namespace std::chrono_literals;
    if (nanoseconds_to_wait_ < nanoseconds_elapsed) {
      nanoseconds_to_wait_ = 0ns;
    } else {
      nanoseconds_to_wait_ -= nanoseconds_elapsed;
    }

    if (nanoseconds_to_wait_ == 0ns) {
      status_ = status::complete;
    }
  }

  [[gnu::pure, nodiscard]] auto is_complete() const noexcept -> bool {
    switch (status()) {
    case status::complete:
    case status::canceled:
      return true;
    case status::paused:
    case status::active:
      return false;
    }
    std::unreachable();
  }

private:
  enum status status_ { status::active };
};
} // namespace jage