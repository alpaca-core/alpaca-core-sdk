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

/// Base class for local inference providers.
/// Providers are responsible for creating and managing stateful sessions. They are typically
/// facades for an underlying inference library. While providers can be used on their own, they are typically used
/// via the `ProviderRegistry` class.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT Provider {
public:
    virtual ~Provider();

    struct Info {
        /// Human-readable name of the provider.
        /// Does not necessarily have to be unique across providers.
        std::string name;

        /// Optional human readable name of the provider vendor.
        std::string vendor;

        /// Schema for the provider.
        Dict schema;

        /// Additional tags that can be used to filter providers
        std::vector<std::string> tags;

        /// Additional metadata that can be used to store more structured information
        Dict metadata;

        /// Provider-specific raw data which can be used to store additional information.
        /// Use this only as a last resort. You must make sure you know what's in there.
        void* rawData = nullptr;
    };

    /// Info of the provider.
    virtual const Info& info() const noexcept = 0;

    /// Check if the model can be loaded
    /// This function is used by `ProviderRegistry` to check if the model should be loaded by this provider.
    /// Keep it as lightweight as possible.
    virtual bool canLoadModel(const ModelAssetDesc& desc, const Dict& params) const noexcept = 0;

    /// Load a model based on the provided description and parameters.
    /// The progress callback is optional and can be used to report the progress of the loading process.
    /// The returned model is owned by the caller and is not bound to the provider in any way.
    virtual ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb) = 0;
};

} // namespace ac::local
