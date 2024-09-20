// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class Sandbox {
    static {
        System.loadLibrary("ac-jni");
    }

    public Sandbox(int i) {
        initialize(i);
    }

    private long nativePtr;
    protected native void initialize(int i);
    protected native void finalize() throws Throwable;

    public native int add(int a, int b);
    public native void seti(int i);
}
