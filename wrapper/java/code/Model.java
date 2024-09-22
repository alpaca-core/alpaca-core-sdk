// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import java.util.Map;

public interface Model {
    public interface CreateInstanceCallback {
        void onComplete(Instance instance);
        void onError(String error);
    }
    public void createInstance(String type, Map params, CreateInstanceCallback cb);
}
