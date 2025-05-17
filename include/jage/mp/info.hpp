#pragma once

namespace jage::mp {
template <auto id> struct info {
#pragma GCC diagnostic push
#if !defined(__clang__)
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif
  constexpr auto friend get(info);

#pragma GCC diagnostic pop
};
} // namespace jage::mp