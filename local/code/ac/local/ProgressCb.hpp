// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <itlib/ufunction.hpp> // C++23: replace with std::move_only_function
#include <string_view>

namespace ac::local {

/**
 * @brief Type alias for a progress callback function.
 *
 * This function is called to report progress during the asynchronous operation.
 * @param tag A string view representing a tag or category for the progress update.
 *            Since async tasks can be internally composed of multiple independent
 *            async tasks, tags allow for more granular progress reporting.
 * @param progress A float between 0 and 1
 */
using ProgressCb = itlib::ufunction<void(std::string_view tag, float progress)>;

} // namespace ac::local
