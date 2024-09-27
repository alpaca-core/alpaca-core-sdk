// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public class Model {
    public native Instance createInstance(String type, Object params);

    private long nativePtr;
}
