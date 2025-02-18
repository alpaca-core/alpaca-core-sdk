// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../stream_result.hpp"
#include <ac/xec/task.hpp>

namespace ac::xec {
class notifiable;
}

namespace ac::io {

template <typename T>
concept on_blocked_cb_class =
    std::same_as<T, std::nullptr_t>
    || requires(T f) {
        { std::invoke(f) } -> std::convertible_to<xec::task>;
    };

template <typename T>
concept close_class = requires(T& s) {
    { s.close() };
};

template <typename T>
concept read_class =
    close_class<T>
    && requires {
        typename T::read_value_type;
    }
    && requires(T& s, typename T::read_value_type& val, xec::task (*on_blocked)()) {
        { s.read(val, on_blocked) } -> std::same_as<stream_result>;
        { s.read(val, nullptr) } -> std::same_as<stream_result>;
    };

template <typename T>
concept write_class =
    close_class<T>
    && requires {
        typename T::write_value_type;
    }
    && requires(T& s, typename T::write_value_type& val, xec::task (*on_blocked)()) {
        { s.write(val, on_blocked) } -> std::same_as<stream_result>;
        { s.write(val, nullptr) } -> std::same_as<stream_result>;
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
