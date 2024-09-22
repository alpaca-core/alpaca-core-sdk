// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import java.util.Map;

class ModelImpl implements Model {
    long nativePtr;
    private ModelImpl(long ptr) {
        nativePtr = ptr;
    }
    @Override
    protected native void finalize() throws Throwable;

    @Override
    public native void createInstance(String type, Map params, CreateInstanceCallback cb);
}

class InstanceImpl implements Instance {
    long nativePtr;
    private InstanceImpl(long ptr) {
        nativePtr = ptr;
    }
    @Override
    protected native void finalize() throws Throwable;

    @Override
    public native void runOp(String op, Map params, OpCallback cb);

    @Override
    public native void synchronize();

    @Override
    public native void initiateAbort(AbortCallback cb);
}
