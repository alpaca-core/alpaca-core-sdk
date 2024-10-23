// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <itlib/expected.hpp>
#include <astl/move.hpp>

template <typename T, typename E>
class Expected : public itlib::expected<T, E> {
    using super = itlib::expected<T, E>;
public:
    using super::expected;
    T consumeValue() noexcept {
        return astl::move(super::value());
    }

    E error() const noexcept {
        return super::error();
    }

    bool hasValue() const {
        return super::has_value();
    }

    bool hasError() const {
        return super::has_error();
    }
};
