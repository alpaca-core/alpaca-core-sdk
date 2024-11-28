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

template <int I>
struct switch_index {
    template <typename Tuple, typename VFunc>
    static constexpr decltype(auto) switch_case(Tuple& t, int i, VFunc&& f) {
        if (i == I) {
            return f(std::get<I>(t));
        }
        else {
            return switch_index<I - 1>::switch_case(t, i, std::forward<VFunc>(f));
        }
    }
};

template <>
struct switch_index<-1> {
    template <typename Tuple, typename VFunc>
    static constexpr auto switch_case(Tuple&, int, VFunc&& f) -> decltype(f()) {
        return f();
    }
};

} // namespace impl

template <typename Tuple, typename VFunc>
constexpr decltype(auto) switch_index(Tuple& t, int i, VFunc&& f) {
    return impl::switch_index<std::tuple_size_v<Tuple> - 1>::switch_case(t, i, std::forward<VFunc>(f));
}

template <typename T, typename Tuple, typename VFunc>
constexpr decltype(auto) switch_type(Tuple& t, VFunc&& f) {
    return switch_index(t, type_index_v<T, Tuple>, std::forward<VFunc>(f));
}

} // namespace astl::tuple
