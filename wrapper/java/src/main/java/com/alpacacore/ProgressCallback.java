// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

public interface ProgressCallback {
    public boolean onProgress(String tag, float progress);
}
