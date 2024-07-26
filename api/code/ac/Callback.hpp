// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Error.hpp"
#include <itlib/expected.hpp>
#include <functional>
#include <string_view>

namespace ac {

template <typename R, typename S = float>
struct Callback {
    using ResultCb = std::function<void(itlib::expected<R, Error>)>;
    ResultCb resultCb;
    using StreamCb = std::function<void(S)>;
    StreamCb progressCb;
};

}
