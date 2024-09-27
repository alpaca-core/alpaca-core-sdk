// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public interface Instance {
    public Object runOp(String op, Object params, ProgressCallback cb);

    private long nativePtr;
}
