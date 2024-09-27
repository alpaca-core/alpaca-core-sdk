// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public class Instance {
    public native Object runOp(String op, Object params, ProgressCallback cb);

    private long nativePtr;
    private Instance() {}
}
