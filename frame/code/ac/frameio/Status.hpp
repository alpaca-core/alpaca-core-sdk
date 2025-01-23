// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <bitset>

namespace ac::frameio {
struct Status {
    enum Bits {
        Success,
        Timeout,
        Aborted,
        Closed,

        Num_Bits
    };
    std::bitset<Num_Bits> bits;

    Status& set(Bits bit) {
        bits[bit] = true;
        return *this;
    }

    Status& reset() {
        bits.reset();
        return*this;
    }

    Status& setBlocked() { return reset(); }
    Status& setSuccess() { return set(Success); }
    Status& setTimeout() { return set(Timeout); }
    Status& setAborted() { return set(Aborted); }
    Status& setClosed() { return set(Closed); }

    bool success() const noexcept { return bits[Success]; }
    bool timeout() const noexcept { return bits[Timeout]; }
    bool aborted() const noexcept { return bits[Aborted]; }
    bool closed() const noexcept { return bits[Closed]; }

    bool blocked() const noexcept { return !success() && !aborted() && !timeout(); }
    bool complete() const noexcept { return success() || closed(); }

    Status& operator|=(const Status& other) {
        bits |= other.bits;
        return *this;
    }
};

} // namespace ac::frameio
