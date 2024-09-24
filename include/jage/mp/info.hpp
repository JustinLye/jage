#pragma once

namespace jage::mp {
template <auto id> struct info {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
  constexpr auto friend get(info);
#pragma GCC diagnostic pop
};
} // namespace jage::mp