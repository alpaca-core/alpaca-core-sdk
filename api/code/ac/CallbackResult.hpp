// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Error.hpp"
#include <itlib/expected.hpp>

namespace ac {

/**
 * @brief A type alias for the result of a callback operation.
 *
 * @tparam R The type of the successful result.
 */
template <typename R>
using CallbackResult = itlib::expected<R, Error>;

}