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
 * This function is called by the SDK to report progress during a synchronous local operation.
 *
 * @param tag A string view representing a tag or category for the progress update. A task may be composed of multiple
 *            unrelated sub-tasks, each with its own progress. The tag can be used to differentiate them.
 * @param progress A float between 0 and 1 representing the progress of the task. 0 indicates no progress, and 1
 *                 indicates completion.
 *
 * @return  A boolean value indicating whether the operation should continue. Returning false can be used to cancel
 *          the operation.
 */
using ProgressCb = itlib::ufunction<bool(std::string_view tag, float progress)>;

} // namespace ac::local
