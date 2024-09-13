// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Info.hpp"
#include <string>
#include <string_view>
#include <memory>
#include <functional>

namespace ac::asset {

class Source;

class AC_ASSET_EXPORT Manager {
public:
    Manager();
    ~Manager();

    void addSource(std::unique_ptr<Source> source, int priority = 0);

    using QueryAssetCb = std::function<void(std::string_view id, const Info& data)>;
    void queryAsset(std::string id, QueryAssetCb cb);

    // return false from progressCb to abort the get
    using GetAssetCb = std::function<void(std::string_view id, const Info& data)>;
    using GetAssetProgressCb = std::function<bool(std::string_view id, float progress)>;
    void getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb);
public:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::asset
