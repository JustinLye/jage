#pragma once

#include <jage/input/key_status.hpp>
#include <jage/input/keyboard_state.hpp>
#include <jage/input/keys.hpp>

#include <array>
#include <bitset>
#include <cstddef>
#include <functional>
#include <optional>
#include <range/v3/all.hpp>
#include <stdexcept>
#include <utility>

namespace jage::input {
template <class TDriver, std::size_t CallbackCapacity = 2> class keyboard {
  using callback_t = std::optional<std::function<void(const keyboard_state &)>>;
  std::reference_wrapper<TDriver> driver_;
  std::array<callback_t, CallbackCapacity> callbacks_{};
  std::bitset<static_cast<std::size_t>(std::to_underlying(keys::END) + 1)>
      monitored_keys_{};
  keyboard_state keyboard_state_{};

  auto update_state_() -> void {
    for (auto index = 0UZ; index < std::size(monitored_keys_); ++index) {
      if (monitored_keys_[index]) {
        if (const auto key = static_cast<keys>(index);
            driver_.get().is_down(key)) {
          keyboard_state_[key].status = key_status::down;
        }
      }
    }
  }

  auto monitor_input_(const keys key) -> void {
    monitored_keys_[static_cast<std::size_t>(std::to_underlying(key))] = true;
  }

  static constexpr auto has_value = [](const callback_t &callback) -> bool {
    return callback.has_value();
  };

public:
  class callback_registration {
    friend keyboard<TDriver, CallbackCapacity>;
    std::reference_wrapper<callback_t> callback_;

  public:
    explicit callback_registration(callback_t &callback)
        : callback_{callback} {}

    auto deregister() -> void { callback_.get().reset(); }
  };

  explicit keyboard(TDriver &driver) : driver_{driver} {}

  auto monitor_input(const auto... key) -> void {
    (monitor_input_(static_cast<keys>(key)), ...);
  }

  auto monitor_input(const std::ranges::range auto &keys) -> void {
    for (const auto &key : keys) {
      monitor_input_(key);
    }
  }

  [[nodiscard]] auto
  register_callback(auto &&callback_to_register) -> callback_registration {
    for (auto &callback : callbacks_) {
      if (not callback.has_value()) {
        callback.emplace(
            std::forward<decltype(callback_to_register)>(callback_to_register));
        return callback_registration{callback};
      }
    }
    throw std::runtime_error(
        "Error! Cannot register callback because callbacks are at capacity.");
  }

  auto poll() -> void {
    if (auto callback_iter = std::find_if(std::begin(callbacks_),
                                          std::end(callbacks_), has_value);
        std::end(callbacks_) != callback_iter) {
      update_state_();
      callback_iter->value()(keyboard_state_);
      std::for_each(std::next(callback_iter), std::end(callbacks_),
                    [&](callback_t &callback) -> void {
                      if (callback.has_value())
                        callback.value()(keyboard_state_);
                    });
    }
  }
};
} // namespace jage::input