// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac::io {

template <typename Notifiable>
auto notify_one_on_blocked(Notifiable& n) {
    return [&n]() {
        return [&n]() {
            n.notify_one();
        };
    };
}

} // namespace ac::io
