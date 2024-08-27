// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "AssetInfo.hpp"
#include <string_view>
#include <memory>
#include <funtional>

namespace ac {

class AssetManagerLayer;

class AC_LOCAL_EXPORT AssetManager {
public:
    AssetManager();
    ~AssetManager();

    void queryAsset(std::string_view id,
        std::function<void(std::string_view id, const AssetInfo& data)> cb);

    void getAsset(std::string_view id,
        std::function<void(std::string_view id, const AssetInfo& data, float progress)> cb,
        bool fetch = true);

public:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
