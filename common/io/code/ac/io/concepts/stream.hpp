// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../stream_result.hpp"

namespace ac::xec {
class notifiable;
}

namespace ac::io {

template <typename T>
concept close_class = requires(T s) {
    { s.close() };
};

template <typename T>
concept read_class =
    close_class<T>
    && requires {
        typename T::read_value_type;
    }
    && requires(T s, typename T::read_value_type& val, xec::notifiable* nobj) {
        { s.read(val, nobj) } -> std::same_as<stream_result>;
    };

template <typename T>
concept write_class =
    close_class<T>
    && requires {
        typename T::write_value_type;
    }
    && requires(T s, typename T::write_value_type& val, xec::notifiable* nobj) {
        { s.write(val, nobj) } -> std::same_as<stream_result>;
    };

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
