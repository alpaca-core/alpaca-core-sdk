// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/Dict.hpp>

namespace ac::local {

/// Service info. Typically shared between service factories and instances
struct ServiceInfo {
    /// Human-readable name of the service.
    /// Does not necessarily have to be unique across services (though it is a good idea to strive for uniqueness).
    std::string name;

    /// Optional human readable name of the service vendor.
    std::string vendor;

    /// Schema for the service.
    Dict schema;

    /// Additional tags that can be used to filter services
    std::vector<std::string> tags;

    /// Additional metadata that can be used to store more structured information
    Dict metadata;

    /// Service-specific raw data which can be used to store additional information.
    /// Use this only as a last resort. You must make sure you know what's in there.
    void* rawData = nullptr;
};

} // namespace ac::local
