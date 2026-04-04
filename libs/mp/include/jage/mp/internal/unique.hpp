#pragma once

#include <jage/mp/contains.hpp>
#include <jage/mp/if_then_else.hpp>

namespace jage::mp::internal {

template <class, class...> struct set_insert;

template <template <class...> class TSet, class... T>
struct set_insert<TSet<T...>> {
  using type = TSet<T...>;
};

template <template <class...> class TList, class... TUnique, class TCandidate,
          class... TRemainingCandidates>
struct set_insert<TList<TUnique...>, TCandidate, TRemainingCandidates...> {
  static constexpr auto candidate_in_set =
      contains<TCandidate, TList<TUnique...>>;
  using current_set_type = if_then_else<candidate_in_set, TList<TUnique...>,
                                        TList<TUnique..., TCandidate>>::type;
  using type = set_insert<current_set_type, TRemainingCandidates...>::type;
};

template <class, class...> struct unique;
template <template <class...> class TList, class... T>
struct unique<TList<T...>> {
  using type = set_insert<TList<>, T...>::type;
};

} // namespace jage::mp::internal