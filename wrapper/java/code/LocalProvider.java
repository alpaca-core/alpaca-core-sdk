// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import java.util.Map;

public class LocalProvider {
    static {
        System.loadLibrary("ac-jni");
    }

    public interface LoadModelCallback {
        void onComplete(Model model);
        void onError(String error);
        void onProgress(String tag, float progress);
    }

    static native void loadModel(ModelDesc desc, Map params, LoadModelCallback cb);
}
