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
    enum InitFlags : uint32_t {
        None = 0,

        // if the manager does not launch it's own thread, it is the responsibility of the user to call
        // run and pushStop/abortRun appropriately - within the lifetime of the manager
        No_LaunchThread = 1,
    };

    explicit Manager(uint32_t flags = 0);
    ~Manager();

    void addSource(std::unique_ptr<Source> source, int priority = 0);

    using QueryAssetCb = std::function<void(std::string_view id, const Info& data)>;
    void queryAsset(std::string id, QueryAssetCb cb);

    // return false from progressCb to abort the get
    using GetAssetCb = std::function<void(std::string_view id, const Info& data)>;
    using GetAssetProgressCb = std::function<bool(std::string_view id, float progress)>;
    void getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb);

    //////////////////////////////////////////////////////////////////////////
    // no thread functions
    void run(); // blocks the current thread until stop is called
    void abortRun(); // stops the manager execution potentially aborting any in-flight operations
    void pushStop(); // stops the manager execution after all in-flight operations are completed
public:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace ac::asset
