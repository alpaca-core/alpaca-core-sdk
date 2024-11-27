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
class ModelLoader;
struct PluginInfo;
class ModelLoaderScorer;

class AC_LOCAL_EXPORT ModelLoaderRegistry {
public:
    ModelLoaderRegistry(std::string_view name = {});
    ModelLoaderRegistry(const ModelLoaderRegistry&) = delete;
    ModelLoaderRegistry& operator=(const ModelLoaderRegistry&) = delete;

    const std::string& name() const noexcept { return m_name; }

    struct LoaderData {
        ModelLoader* loader; // never null
        PluginInfo* plugin; // may be null for loaders that have been added directly
    };

    const std::vector<LoaderData>& loaders() const noexcept { return m_loaders; }

    void addLoader(ModelLoader& loader, PluginInfo* plugin = nullptr);
    void removeLoader(ModelLoader& loader);

    // find the best loader for the given model description and parameters
    // returns nullptr if all loaders rank equal or lower then the denyScore of the scorer
    ModelLoader* findBestLoader(const ModelLoaderScorer& scorer, const ModelAssetDesc& desc, const Dict& params) const;

    // utliity functions to directly load the model

    // load model with the first loader which can load it
    ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb = {}) const;

    // load model with a scorer to select the best loader
    ModelPtr loadModel(const ModelLoaderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb = {}) const;
private:
    std::string m_name;
    std::vector<LoaderData> m_loaders;
};

} // namespace ac::local
