// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Dict.hpp>

namespace ac::local {

struct ProviderSessionContext;

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

    virtual void createSession(ProviderSessionContext context) = 0;
};

} // namespace ac::local
