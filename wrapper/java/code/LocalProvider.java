// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class LocalProvider {
    static {
        System.loadLibrary("ac-jni");
    }

    public interface LoadModelCallback {
        void onComplete(Model model);
        void onError(String error);
        void onProgress(String tag, float progress);
    }

    public static native void loadModel(ModelDesc desc, Object params, LoadModelCallback cb);

    public static native void sandbox(ModelDesc desc);
}
