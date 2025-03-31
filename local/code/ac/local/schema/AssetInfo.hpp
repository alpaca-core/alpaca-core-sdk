// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <string>
#include <vector>

namespace ac::schema {

inline namespace amgr {

struct AssetInfo {
    Field<std::string> tag;
    Field<std::string> uri;

    template <typename Visitor>
    void visitFields(Visitor& v) {
        v(tag, "tag", "Asset tag/name");
        v(uri, "uri", "Asset URI");
    }
};

using AssetInfos = std::vector<AssetInfo>;

} // namespace amgr

} // namespace ac::schema
