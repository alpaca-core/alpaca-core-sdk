// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class Dict {
    private long nativePtr;
    protected native void initialize();

    @Override
    protected native void finalize() throws Throwable;

    public Dict() {
        initialize();
    }

    public static native Dict parseJson(String json);

    @Override
    public native Dict clone();

    public native Dict atKey(String key) throws Exception;
    public native Dict atIndex(int index) throws Exception;

    enum ValueType {
        NULL,
        BOOL,
        INT,
        UNSIGNED,
        DOUBLE,
        STRING,
        ARRAY,
        OBJECT,
        BINARY
    }

    public native ValueType type();

    public native int size();

    public native boolean getBool() throws Exception;
    public native int getInt() throws Exception;
    public native long getUnsigned() throws Exception;
    public native double getDouble() throws Exception;
    public native String getString() throws Exception;
    // binary?

    public native void setValue(boolean value);
    public native void setValue(int value);
    public native void setValue(long value);
    public native void setValue(double value);
    public native void setValue(String value);
    // binary?

    public native void setArray();
    public native void setObject();

    public native Dict addChild(String key) throws Exception;
    public native Dict addItem() throws Exception;

    public native String dump(int indent);
    String dump() {
        return dump(0);
    }
}
