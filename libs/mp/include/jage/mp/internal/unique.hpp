#pragma once

#include <jage/mp/contains.hpp>
#include <jage/mp/list.hpp>

#include <type_traits>

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
  using current_set_type =
      std::conditional_t<candidate_in_set, TList<TUnique...>,
                         TList<TUnique..., TCandidate>>;
  using type = set_insert<current_set_type, TRemainingCandidates...>::type;
};

template <class... Ts> struct unique {
  using type = set_insert<list<>, Ts...>::type;
};

template <template <class...> class TList, class... T>
struct unique<TList<T...>> {
  using type = set_insert<TList<>, T...>::type;
};

} // namespace jage::mp::internal