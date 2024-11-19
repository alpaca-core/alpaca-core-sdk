// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <compare>

namespace astl {

struct version {
    int major;
    int minor;
    int patch;

    constexpr auto operator<=>(const version&) const noexcept = default;

    static constexpr version from_int(int v) noexcept {
        return version{v/1'000'000, (v / 1'000) % 1'000, v % 1'000 };
    }

    constexpr int to_int() const noexcept {
        return major * 1'000'000 + minor * 1'000 + patch;
    }
};

} // namespace astl
