// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <type_traits>

// copied from https://github.com/iboB/itlib/blob/master/include/itlib/utility.hpp

// function which guarantees that a move happens
// if the move would result in copy construction or assignment, the call won't compile
namespace astl {
template <typename T>
constexpr typename std::remove_reference<T>::type&& move(T&& t) noexcept {
    using NoRefT = typename std::remove_reference<T>::type;
    static_assert(!std::is_const<NoRefT>::value, "cannot move a const object");
    return static_cast<NoRefT&&>(t);
}
}
