// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <bitset>

namespace ac::frameio {
struct Status {
    enum Bits {
        Success,
        Closed,

        Waiting,
        Aborted,

        Timeout,

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

    Status& setSuccess() { return set(Success); }
    Status& setClosed() { return set(Closed); }

    Status& setAborted() { return set(Aborted); }
    Status& setWaiting() { return set(Waiting); }

    Status& setTimeout() { return set(Timeout); }

    bool success() const noexcept { return bits[Success]; }
    bool closed() const noexcept { return bits[Closed]; }
    bool blocked() const noexcept { return !success() && !closed(); }
    bool complete() const noexcept { return success() || closed(); }

    bool waiting() const noexcept { return bits[Waiting]; }
    bool aborted() const noexcept { return bits[Aborted]; }

    bool timeout() const noexcept { return bits[Timeout]; }

    Status& operator|=(const Status& other) {
        bits |= other.bits;
        return *this;
    }
};

} // namespace ac::frameio
