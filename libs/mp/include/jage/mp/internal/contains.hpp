#pragma once

#include <concepts>

namespace jage::mp::internal {

template <class TTarget, class... TCandidates>
static constexpr auto contains = (std::same_as<TTarget, TCandidates> or ...);

template <class TTarget, template <class...> class TList, class... TCandidates>
static constexpr auto contains<TTarget, TList<TCandidates...>> =
    contains<TTarget, TCandidates...>;

} // namespace jage::mp::internal