// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <type_traits>

// function which guarantees that a move happens
// if the move would result in copy construction or assignment, the call won't compile
namespace astl {
template <typename T>
constexpr std::remove_reference_t<T>&& move(T&& t) noexcept {
    static_assert(!std::is_const_v<std::remove_reference_t<T>>, "cannot move a const object");
    return static_cast<typename std::remove_reference_t<T>&&>(t);
}
}
