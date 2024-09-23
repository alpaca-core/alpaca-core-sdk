// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public interface Instance {
    public interface OpCallback {
        void onComplete();
        void onError(String error);
        void onProgress(String tag, float progress);
        void onStream(Object data);
    }
    public void runOp(String op, Object params, OpCallback cb);

    public void synchronize();

    public interface AbortCallback {
        void onAbort();
    }
    public void initiateAbort(AbortCallback cb);
}
