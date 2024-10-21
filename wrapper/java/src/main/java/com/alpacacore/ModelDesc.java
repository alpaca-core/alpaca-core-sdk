// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

/// An descriptor for models.
/// @ingroup java-local
public class ModelDesc {
    /// Default constructor for an empty description.
    public ModelDesc() {}

    /// Constructor from components.
    public ModelDesc(String inferenceType, AssetInfo[] assets, String name) {
        this.inferenceType = inferenceType;
        this.assets = assets;
        this.name = name;
    }

    /// Inference type.
    /// Used as a router to the correct underlying inference library.
    public String inferenceType;

    /// Information for an individual asset.
    public static final class AssetInfo {
        public String path; ///< Path to the asset.
        public String tag; ///< Asset tag.
    }
    public AssetInfo[] assets; ///< Model assets.

    /// Name tag.
    /// This field is not used by the library in any way besides logs and may be helpful for debugging.
    public String name;
}
