#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <range/v3/all.hpp>
#include <stdexcept>
#include <utility>

namespace jage::input::generic {

template <class TDriver, std::size_t CallbackCapacity, class TState,
          auto UpdateStateFunct, auto InvokeCallbackFunct, class TCallbackParam>
class pollster {
public:
  using callback_t = std::optional<std::function<void(const TCallbackParam &)>>;

private:
  std::reference_wrapper<TDriver> driver_;
  std::array<callback_t, CallbackCapacity> callbacks_{};

  static constexpr auto has_value = [](const callback_t &callback) -> bool {
    return callback.has_value();
  };

protected:
  TState state_{};
  ~pollster() = default;

public:
  class callback_registration {
    std::reference_wrapper<callback_t> callback_;

  public:
    explicit callback_registration(callback_t &callback)
        : callback_{callback} {}

    auto deregister() -> void { callback_.get().reset(); }
  };

  explicit pollster(TDriver &driver) : driver_{driver} {}

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
      UpdateStateFunct(driver_.get(), state_);
      InvokeCallbackFunct(callback_iter->value(), state_);
      std::for_each(std::next(callback_iter), std::end(callbacks_),
                    [&](callback_t &callback) -> void {
                      if (callback.has_value())
                        InvokeCallbackFunct(callback.value(), state_);
                    });
    }
  }
};

} // namespace jage::input::generic