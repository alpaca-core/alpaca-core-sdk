// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "channel.hpp"

namespace ac::io {

template <typename T>
concept stream_class =
    close_class<T>
    && requires {
        typename T::value_type;
    };

template <typename T>
concept read_stream_class = stream_class<T> && read_class<T>;

template <typename T>
concept write_stream_class = stream_class<T> && write_class<T>;

} // namespace ac::io
