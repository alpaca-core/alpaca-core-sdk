// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <astl/safe_func.hpp>
#include <itlib/expected.hpp>
#include <string_view>
#include <functional>
#include <optional>
#include <string>

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

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) = 0;

    using ProgressCb = astl::safe_func<std::function<void(float)>>;
    virtual itlib::expected<BasicAssetInfo, std::string> fetchAssetSync(std::string_view id, ProgressCb progressCb) = 0;
};

} // namespace ac
