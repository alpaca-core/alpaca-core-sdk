// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <itlib/expected.hpp>
#include <string_view>
#include <funtional>

namespace ac {

class AC_LOCAL_EXPORT AssetSource {
public:
    virtual ~AssetSource();

    virtual std::string_view id() const = 0;

    // synchronous interface for now (may be redesigned later)

    struct BasicAssetInfo {
        std::optional<size_t> size;
        std::optional<std::string> path;
    };

    BasicAssetInfo checkAssetSync(std::string_view id) = 0;
    itlib::expected<BasicAssetInfo, std::string> fetchAssetSync(std::string_view id, std::function<float> progressCb) = 0;
};

}
