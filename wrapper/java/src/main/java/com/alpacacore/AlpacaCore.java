// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

/// @defgroup java-local Java Local API
/// Java API for local infernence.

/// SDK Instance.
/// An entry point to using the SDK.
/// @ingroup java-local
public class AlpacaCore {
    static {
        System.loadLibrary("ac-jni");
    }

    /// Create a model from a model description.
    /// The progress callback is optional.
    public static native Model createModel(ModelDesc desc, Object params, ProgressCallback cb);

    /// Release a model.
    /// Models are managed manually not releasing a model is considered a leak.
    public static native void releaseModel(Model model);

    /// Release a model instance.
    /// Instances are managed manually not releasing a instance is considered a leak.
    public static native void releaseInstance(Instance instance);
}
