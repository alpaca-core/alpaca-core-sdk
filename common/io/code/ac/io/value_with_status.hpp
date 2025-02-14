// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "status.hpp"

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
};

template <typename T>
struct value_ref_with_status : public status {
    explicit value_ref_with_status(T& val) : value(val) {}
    value_ref_with_status(T& val, status status) : status(status), value(val) {}

    status& s() { return *this; }
    const status& s() const { return *this; }

    T& value;
};

} // namespace ac::io
