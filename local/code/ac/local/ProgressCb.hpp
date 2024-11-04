// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/ufunction.hpp> // C++23: replace with std::move_only_function
#include <string_view>

namespace ac::local {

/// Type alias for a progress callback function.
///
/// This function is called by the SDK to report progress during a synchronous local operation.
///
/// It uses `astl::ufunction` which is a C++11 implementation of C++23's `std::move_only_function` (ie you can capture
/// move-only types).
///
/// @param tag A string view representing a tag or category for the progress update. Since a task may be composed of
///            multiple unrelated sub-tasks, each with its own progress, the tag can be used to differentiate them.
/// @param progress A float between 0 and 1.
/// @return A boolean value indicating whether the operation should continue. Return false to cancel the operation.
/// @ingroup cpp-local
using ProgressCb = astl::ufunction<bool(std::string_view tag, float progress)>;

} // namespace ac::local
