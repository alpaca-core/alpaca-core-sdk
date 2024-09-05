// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetInfo.hpp"
#include <string>
#include <string_view>
#include <memory>
#include <functional>

namespace ac {

class AssetSource;

class AC_LOCAL_EXPORT AssetManager {
public:
    AssetManager();
    ~AssetManager();

    void addSource(std::unique_ptr<AssetSource> source, int priority = 0);

    using QueryAssetCb = std::function<void(std::string_view id, const AssetInfo& data)>;
    void queryAsset(std::string id, QueryAssetCb cb);

    using GetAssetCb = std::function<void(std::string_view id, const AssetInfo& data)>;
    using GetAssetProgressCb = std::function<void(std::string_view id, float progress)>;
    void getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb);
public:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
