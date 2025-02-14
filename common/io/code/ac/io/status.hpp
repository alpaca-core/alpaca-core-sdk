// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <bitset>

namespace ac::io {

struct status {
    enum bit {
        SUCCESS,
        CLOSED,

        WAITING,
        ABORTED,

        TIMEOUT,

        NUM_BITS
    };
    std::bitset<NUM_BITS> bits;

    status& set(bit b) noexcept {
        bits[b] = true;
        return *this;
    }

    status& reset() noexcept {
        bits.reset();
        return*this;
    }

    status& set_success() noexcept { return set(SUCCESS); }
    status& set_closed() noexcept { return set(CLOSED); }

    status& set_aborted() noexcept { return set(ABORTED); }
    status& set_waiting() noexcept { return set(WAITING); }

    status& set_timeout() noexcept { return set(TIMEOUT); }

    bool success() const noexcept { return bits[SUCCESS]; }
    bool closed() const noexcept { return bits[CLOSED]; }
    bool blocked() const noexcept { return !success() && !closed(); }
    bool complete() const noexcept { return success() || closed(); }

    bool waiting() const noexcept { return bits[WAITING]; }
    bool aborted() const noexcept { return bits[ABORTED]; }

    bool timeout() const noexcept { return bits[TIMEOUT]; }

    status& operator|=(const status& other) noexcept {
        bits |= other.bits;
        return *this;
    }
};

} // namespace ac::io
