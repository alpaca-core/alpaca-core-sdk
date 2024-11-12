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

template <typename Q, typename Tulpe>
inline constexpr int type_index_v = type_index<Q, Tulpe>::value;

} // namespace astl::tuple
