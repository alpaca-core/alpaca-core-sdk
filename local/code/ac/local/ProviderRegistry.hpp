// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "ModelAssetDesc.hpp"
#include "ProgressCb.hpp"
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

    // find the best provider for the given model description and parameters
    // returns nullptr if all providers rank equal or lower then the denyScore of the scorer
    Provider* findBestProvider(const ProviderScorer& scorer, const ModelAssetDesc& desc, const Dict& params) const;

    // utliity functions to directly load the model

    // load model with the first provider which can load it
    ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb = {}) const;

    // load model with a scorer to select the best provider
    ModelPtr loadModel(const ProviderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb = {}) const;
private:
    std::string m_name;
    std::vector<ProviderData> m_providers;
};

} // namespace ac::local
