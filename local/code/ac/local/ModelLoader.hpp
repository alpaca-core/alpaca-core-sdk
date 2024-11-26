// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "ModelAssetDesc.hpp"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>

namespace ac::local {

/// Base class for model loaders.
/// Model loaders are responsible for loading models based on the provided description and parameters. They are typically
/// facades for an underlying inference library. While model loaders can be used on their own, they are typically used
/// via the `ModelFactory` class.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT ModelLoader {
public:
    virtual ~ModelLoader();

    struct Info {
        /// Human-readable name of the loader.
        /// Does not necessarily have to be unique across loaders.
        std::string name;

        /// Optional human readable name of the loader vendor.
        std::string vendor;

        /// Supported model types
        /// Note that this only suggests that the loader may be able to load the model. It's not a 100% guarantee
        std::vector<std::string> assetTypes;

        /// Supported inference interfaces for the general instance
        std::vector<std::string> generalInstanceInterfaces;

        /// Additional tags that can be used to filter loaders
        std::vector<std::string> tags;

        /// Loader-specific user data which can be used to store additional information
        void* userData = nullptr;
    };

    /// Info of the loader.
    virtual const Info& info() const noexcept = 0;

    /// Check if the model can be loaded
    /// This function is used by `ModelLoaderRegistry` to check if an attempt to load a model should be made.
    virtual bool canLoadModel(const ModelAssetDesc& desc, const Dict& params) const noexcept = 0;

    /// Load a model based on the provided description and parameters.
    /// The progress callback is optional and can be used to report the progress of the loading process.
    /// The returned model is owned by the caller and is not bound to the loader in any way.
    virtual ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb) = 0;
};

} // namespace ac::local
