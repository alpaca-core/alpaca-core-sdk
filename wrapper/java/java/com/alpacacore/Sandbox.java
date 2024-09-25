// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

enum FromNativeTag {
    FROM_NATIVE
}

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
    private Sandbox(FromNativeTag tag, long ptr) {
        nativePtr = ptr;
    }
    private static Sandbox fromNative(long ptr) {
        return new Sandbox(FromNativeTag.FROM_NATIVE, ptr);
    }

    public native int add(int a, int b);
    public native void seti(int i);
    public native Sandbox clone();
}
