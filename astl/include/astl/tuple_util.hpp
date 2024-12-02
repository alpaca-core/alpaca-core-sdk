// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <tuple>

namespace astl::tuple {

// type index: find index of the first occurrence of type in tuple or -1 if not found
template <typename Q, typename Tuple, int I = 0>
struct type_index;

template <typename Q, int I>
struct type_index<Q, std::tuple<>, I> {
    static inline constexpr int value = -1;
};

template <typename Q, int I, typename... Ts>
struct type_index<Q, std::tuple<Q, Ts...>, I> {
    static inline constexpr int value = I;
};

template <typename Q, int I, typename T, typename... Ts>
struct type_index<Q, std::tuple<T, Ts...>, I> {
    static inline constexpr int value = type_index<Q, std::tuple<Ts...>, I + 1>::value;
};

template <typename Q, typename Tuple>
inline constexpr int type_index_v = type_index<Q, std::remove_cv_t<Tuple>>::value;

namespace impl {
template <int I, typename Tuple, typename QFunc, typename VFunc, typename NFunc>
static constexpr decltype(auto) find_if(Tuple& tup, QFunc& qf, VFunc& vf, NFunc& nf) {
    if constexpr (I == std::tuple_size_v<Tuple>) {
        return nf();
    }
    else if (auto& val = std::get<I>(tup); qf(I, val)) {
        return vf(val);
    }
    else {
        return find_if<I + 1>(tup, qf, vf, nf);
    }
}
}

template <typename Tuple, typename QFunc, typename VFunc, typename NFunc>
static constexpr decltype(auto) find_if(Tuple& tup, QFunc qf, VFunc vf, NFunc nf) {
    return impl::find_if<0>(tup, qf, vf, nf);
}

template <typename Tuple, typename VFunc, typename NFunc>
constexpr decltype(auto) switch_index(Tuple& tup, int i, VFunc vf, NFunc nf) {
    auto qfunc = [i](int j, const auto&) constexpr { return i == j; };
    return impl::find_if<0>(tup, qfunc, vf, nf);
}

} // namespace astl::tuple
