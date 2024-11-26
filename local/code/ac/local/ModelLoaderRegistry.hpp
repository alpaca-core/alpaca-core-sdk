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

class AC_LOCAL_EXPORT ModelLoaderRegistry {
public:
    ModelLoaderRegistry(std::string_view name = {});
    ModelLoaderRegistry(const ModelLoaderRegistry&) = delete;
    ModelLoaderRegistry& operator=(const ModelLoaderRegistry&) = delete;

    const std::string& name() const noexcept { return m_name; }

    struct LoaderData {
        ModelLoader& loader; // never null
        PluginInfo* plugin; // may be null for loaders that have been added directly
    };

    const std::vector<LoaderData>& loaders() const noexcept { return m_loaders; }

    void addLoader(ModelLoader& loader, PluginInfo* plugin = nullptr);
    void addPlugin(PluginInfo& plugin);

    std::optional<LoaderData> findLoader(std::string_view modelAssetType) const noexcept;

    // temp until we figure out better loader queries
    ModelPtr createModel(ModelAssetDesc desc, Dict params, ProgressCb cb = {}) const;
private:
    std::string m_name;
    std::vector<LoaderData> m_loaders;
};

} // namespace ac::local
