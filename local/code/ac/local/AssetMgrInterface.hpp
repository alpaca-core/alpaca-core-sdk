// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
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

    struct AssetInfo {
        Field<std::string> tag;
        Field<std::string> uri;

        template <typename Visitor>
        void visitFields(Visitor& v) {
            v(tag, "tag", "Asset tag/name");
            v(uri, "uri", "Asset URI");
        }
    };

    struct OpMakeAssetsAvailable {
        static constexpr auto id = "make-assets-available";
        static constexpr auto desc = "Make assets available";

        using Params = std::vector<AssetInfo>;
        using Return = std::vector<AssetInfo>;

        using Outs = std::tuple<sys::Progress>;
    };

    using Ops = std::tuple<OpMakeAssetsAvailable>;
};

struct Interface {
    static inline constexpr std::string_view id = "$asset-mgr";
    static inline constexpr std::string_view desc = "Asset manager interface";

    using States = std::tuple<State>;
};

} // namespace amgr

} // namespace ac::schema
