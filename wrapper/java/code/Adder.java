// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class Adder {
    static {
        System.loadLibrary("ac-jni");
    }

    public static native int add(int a, int b);
}
