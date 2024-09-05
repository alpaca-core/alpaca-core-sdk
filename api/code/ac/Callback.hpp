// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Error.hpp"
#include <itlib/expected.hpp>
#include <functional>
#include <string_view>

namespace ac {

template <typename R>
using CallbackResult = itlib::expected<R, Error>;

template <typename R, typename S = float>
struct Callback {
    using ResultCb = std::function<void(CallbackResult<R>)>;
    ResultCb resultCb;
    using StreamCb = std::function<void(std::string_view tag, S)>;
    StreamCb progressCb;
};

}
