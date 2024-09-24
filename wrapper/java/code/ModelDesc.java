// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class ModelDesc {
    public ModelDesc() {}
    public ModelDesc(String inferenceType, AssetInfo[] assets, String name) {
        this.inferenceType = inferenceType;
        this.assets = assets;
        this.name = name;
    }

    public String inferenceType;

    public static class AssetInfo {
        public String path;
        public String tag;
    }
    public AssetInfo[] assets;

    // this field is not used by the library in any way besides logs and may be helpful for debugging
    public String name;
}
