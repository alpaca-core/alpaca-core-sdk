// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <utility>
#include <cstddef>

// safe function
// calling for nullptr is safe and does nothing

namespace astl {

template <typename Func>
struct safe_func {
    Func func;

    safe_func() : func(nullptr) {};
    safe_func(std::nullptr_t) : safe_func() {}

    safe_func(const safe_func&) = default;
    safe_func& operator=(const safe_func&) = default;
    safe_func(safe_func&&) noexcept = default;
    safe_func& operator=(safe_func&&) noexcept = default;

    // intentionally implicit
    template <typename F>
    safe_func(F&& f) : func(std::forward<F>(f)) {}

    template <typename F>
    safe_func& operator=(F&& f) {
        func = std::forward<F>(f);
        return *this;
    }

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
