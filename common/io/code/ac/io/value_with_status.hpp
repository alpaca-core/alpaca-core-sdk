// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "status.hpp"
#include "status_builder.hpp"

namespace ac::io {

template <typename T>
struct value_with_status : public status {
    using value_type = T;

    value_with_status() : value() {}
    explicit value_with_status(T val) : value(std::move(val)) {}
    value_with_status(T val, status s) : status(s), value(std::move(val)) {}

    status& s() { return *this; }
    const status& s() const { return *this; }

    T value;

    void reset(T v = {}) {
        status::reset();
        value = std::move(v);
    }

    template <typename ...Args>
    static status_builder<value_with_status<T>> build(Args&&... args) {
        return status_builder<value_with_status<T>>(std::forward<Args>(args)...);
    }
};

template <typename T>
struct value_ref_with_status : public status {
    explicit value_ref_with_status(T& val) : value(val) {}
    value_ref_with_status(T& val, status s) : status(s), value(val) {}

    status& s() { return *this; }
    const status& s() const { return *this; }

    T& value;
};

} // namespace ac::io
