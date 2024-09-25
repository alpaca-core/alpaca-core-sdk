// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public interface Model {
    public interface CreateInstanceCallback {
        void onComplete(Instance instance);
        void onError(String error);
    }
    public void createInstance(String type, Object params, CreateInstanceCallback cb);
}
