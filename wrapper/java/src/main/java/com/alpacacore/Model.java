// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

/// An inference model.
/// @ingroup java-local
public class Model {
    /// Create an inference istance.
    /// The returned instance is not bound to the model lifetime.
    /// If one only needs a single instance, the model which created it can be released immediately after creation.
    public native Instance createInstance(String type, Object params);

    private long nativePtr;
    private Model() {}
}
