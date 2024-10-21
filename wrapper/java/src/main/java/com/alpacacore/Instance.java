// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

/// An inference instance
/// @ingroup java-local
public class Instance {

    /// Run an operation on the instance.
    /// The optional progress callback can be used to report the progress of the operation, but note that most
    /// operations completely ignore that arg. Whether one makes use of it should be documented in the operation's API.
    ///
    /// The returned Object is a Map which contains the result of the operation as per the operation's API.
    public native Object runOp(String op, Object params, ProgressCallback cb);

    private long nativePtr;
    private Instance() {}
}
