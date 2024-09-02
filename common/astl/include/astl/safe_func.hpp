// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <utility>

// safe function
// calling for nullptr is safe and does nothing

namespace astl {

template <typename Func>
struct safe_func {
    Func func;

    safe_func() : func(nullptr) {};

    // intentionally implicit
    safe_func(Func f) : func(f) {}

    template <typename... Args>
    void operator()(Args&&... args) {
        if (func) {
            func(std::forward<Args>(args)...);
        }
    }
};

template <typename Func>
safe_func(Func) -> safe_func<Func>;

} // namespace astl
