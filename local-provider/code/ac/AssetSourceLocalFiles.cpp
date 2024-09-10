// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetSourceLocalFiles.hpp"
#include <ac/Dict.hpp>
#include <stdexcept>

namespace ac {

AssetSourceLocalFiles::AssetSourceLocalFiles(astl::tsumap<std::string> manifest)
    : m_assetManifest(std::move(manifest))
{
    // fill available assets
}

namespace {
astl::tsumap<std::string> parseManifest(std::string_view jsonManifest) {
    astl::tsumap<std::string> result;
    auto manifest = ac::Dict::parse(jsonManifest);
    for (const auto& i : manifest.items()) {
        result[i.key()] = i.value().get<std::string>();
    }
    return result;
}
} // namespace


AssetSourceLocalFiles::AssetSourceLocalFiles(std::string jsonManifest)
    : AssetSourceLocalFiles(parseManifest(jsonManifest))
{}

std::optional<AssetSource::BasicAssetInfo> AssetSourceLocalFiles::checkAssetSync(std::string_view id) noexcept {
    return {};
}

AssetSource::BasicAssetInfo AssetSourceLocalFiles::fetchAssetSync(std::string_view id, ProgressCb pcb) {
    throw std::runtime_error("Not implemented");
}

} // namespace ac
