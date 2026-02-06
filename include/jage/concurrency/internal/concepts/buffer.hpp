#pragma once

#include <concepts>

namespace jage::concurrency::internal::concepts {
namespace detail {
template <class...> static constexpr auto buffer = false;
template <template <class, template <class> class> class TBuffer, class TEvent,
          template <class> class TAtomic>
static constexpr auto buffer<TBuffer<TEvent, TAtomic>> =
    requires(TBuffer<TEvent, TAtomic> buffer_instance, const TEvent &event) {
      { buffer_instance.read() } -> std::same_as<TEvent>;
      { buffer_instance.write(event) } -> std::same_as<void>;
    };

} // namespace detail
template <class... Ts>
concept buffer = detail::buffer<Ts...>;
} // namespace jage::concurrency::internal::concepts