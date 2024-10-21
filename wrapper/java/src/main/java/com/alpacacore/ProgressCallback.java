// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

/// Callback for progress of SDK ops.
/// @ingroup java-local
public interface ProgressCallback {
    /// Callback function.
    /// @param tag A string view representing a tag or category for the progress update. Since a task may be composed of
    ///            multiple unrelated sub-tasks, each with its own progress, the tag can be used to differentiate them.
    /// @param progress A float between 0 and 1.
    /// @return A boolean value indicating whether the operation should continue. Return false to cancel the operation.
    public boolean onProgress(String tag, float progress);
}
