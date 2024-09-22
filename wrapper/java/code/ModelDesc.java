// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

public class ModelDesc {
    public String inferenceType;

    public class AssetInfo {
        public String path;
        public String tag;
    }
    public AssetInfo[] assets;

    // this field is not used by the library in any way besides logs and may be helpful for debugging
    public String name;
}
