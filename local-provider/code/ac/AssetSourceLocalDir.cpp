// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetSourceLocalDir.hpp"
#include <astl/move.hpp>

#include <sys/stat.h>
#if defined(_WIN32)
#   define stat _stat
#endif


namespace ac {

namespace {
class AssetSourceLocalDir final : public AssetSource {
    std::string m_path;
    std::string m_id;
public:
    AssetSourceLocalDir(std::string path)
        : m_path(astl::move(path))
        , m_id("local-dir: " + m_path)
    {}

    virtual std::string_view id() const {
        return m_id;
    }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) override {
        auto path = m_path + "/" + std::string(id);
        struct stat st;
        if (stat(path.c_str(), &st) == 0) {
            return BasicAssetInfo{st.st_size, astl::move(path)};
        }
        return std::nullopt;
    }
    virtual itlib::expected<BasicAssetInfo, std::string> fetchAssetSync(std::string_view id, std::function<void(float)> progressCb) override {
        auto ret = checkAssetSync(id);
        if (ret) {
            return astl::move(*ret);
        }
        else {
            return itlib::unexpected("Asset not found");
        }
    }
};
}

std::unique_ptr<AssetSource> AssetSourceLocalDir_Create(std::string path) {
    return std::make_unique<AssetSourceLocalDir>(astl::move(path));
}
}
