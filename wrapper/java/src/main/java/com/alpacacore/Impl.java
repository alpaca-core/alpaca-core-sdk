// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

class ModelImpl implements Model {
    long nativePtr;
    private ModelImpl(long ptr) {
        nativePtr = ptr;
    }
    @Override
    protected native void finalize() throws Throwable;

    @Override
    public native void createInstance(String type, Object params, CreateInstanceCallback cb);
}

class InstanceImpl implements Instance {
    long nativePtr;
    private InstanceImpl(long ptr) {
        nativePtr = ptr;
    }
    @Override
    protected native void finalize() throws Throwable;

    @Override
    public native void runOp(String op, Object params, OpCallback cb);

    @Override
    public native void synchronize();

    @Override
    public native void initiateAbort(AbortCallback cb);
}
