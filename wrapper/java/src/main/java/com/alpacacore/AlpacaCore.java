// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public class AlpacaCore {
    static {
        System.loadLibrary("ac-jni");
    }

    public static native Model createModel(ModelDesc desc, Object params, ProgressCallback cb);

    public static native void releaseModel(Model model);
    public static native void releaseInstance(Instance instance);
}
