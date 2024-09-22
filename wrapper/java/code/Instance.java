// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import java.util.Map;

public interface Instance {
    public interface OpCallback {
        void onComplete();
        void onError(String error);
        void onProgress(String tag, float progress);
        void onStream(Map data);
    }
    public void runOp(String op, Map params, OpCallback cb);

    public void synchronize();

    public interface AbortCallback {
        void onAbort();
    }
    public void initiateAbort(AbortCallback cb);
}
