// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderInfo.hpp"
#include <vector>

/// @defgroup cpp-local C++ Local API
/// C++ API for local inference.

namespace ac::local {

class AC_LOCAL_EXPORT ModelLoaderRegistry {
public:
    ModelLoaderRegistry() = default;
    ModelLoaderRegistry(const ModelLoaderRegistry&) = delete;
    ModelLoaderRegistry& operator=(const ModelLoaderRegistry&) = delete;

    std::vector<ModelLoaderInfo> loaders;

    void addLoaders(std::vector<ModelLoaderInfo> loaders);

    const ModelLoaderInfo* findLoader(std::string_view schemaType) const;

    // temp until we figure out better loader queries
    ModelPtr createModel(ModelDesc desc, Dict params, ProgressCb cb = {});
};

} // namespace ac::local
