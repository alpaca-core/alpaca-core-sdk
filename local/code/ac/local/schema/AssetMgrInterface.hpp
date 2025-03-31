// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "AssetInfo.hpp"
#include <ac/schema/Field.hpp>
#include <ac/schema/Progress.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

inline namespace amgr {

struct State {
    static constexpr auto id = "asset-mgr";
    static constexpr auto desc = "Asset manager state";

    struct OpMakeAssetsAvailable {
        static constexpr auto id = "make-assets-available";
        static constexpr auto desc = "Make assets available";

        using Params = AssetInfos;
        using Return = AssetInfos;

        using Outs = std::tuple<sys::Progress>;
    };

    using Ops = std::tuple<OpMakeAssetsAvailable>;
};

struct Interface {
    static inline constexpr auto id = "$asset-mgr";
    static inline constexpr auto desc = "Asset manager interface";

    using States = std::tuple<State>;
};

} // namespace amgr

} // namespace ac::schema
