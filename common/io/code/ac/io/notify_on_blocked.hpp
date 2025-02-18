// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/xec/notifiable.hpp>

namespace ac::io {

inline auto notify_on_blocked(xec::notifiable& n) {
    return [&n]() {
        return [&n]() {
            n.notify_one();
        };
    };
}

} // namespace ac::io
