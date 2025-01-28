// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/frameio/SessionHandlerPtr.hpp>
#include <ac/Dict.hpp>
#include <vector>
#include <optional>

/// @defgroup cpp-local C++ Local API
/// C++ API for local inference.

namespace ac::local {
class Provider;
struct PluginInfo;
class ProviderScorer;

class AC_LOCAL_EXPORT ProviderRegistry {
public:
    ProviderRegistry(std::string_view name = {});
    ProviderRegistry(const ProviderRegistry&) = delete;
    ProviderRegistry& operator=(const ProviderRegistry&) = delete;

    const std::string& name() const noexcept { return m_name; }

    struct ProviderData {
        Provider* provider; // never null
        PluginInfo* plugin; // may be null for providers that have been added directly
    };

    const std::vector<ProviderData>& providers() const noexcept { return m_providers; }

    void addProvider(Provider& provider, PluginInfo* plugin = nullptr);
    void removeProvider(Provider& provider);

    // find the best provider
    // returns nullptr if all providers rank equal or lower then the denyScore of the scorer
    Provider* findBestProvider(const ProviderScorer& scorer) const noexcept;

    // load model with the first provider whose name matches matchName
    frameio::SessionHandlerPtr createSessionHandler(std::string_view matchName);
private:
    std::string m_name;
    std::vector<ProviderData> m_providers;
};

} // namespace ac::local
