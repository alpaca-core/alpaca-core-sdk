// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_provider.h"
#include "LocalProvider.hpp"
#include "ModelInfo.hpp"
#include <ac/asset/Source.hpp>
#include <ac/ApiCUtil.hpp>

extern "C" {

ac_api_provider* ac_new_local_api_provider() {
    return ac::cutil::Provider_to_provider(new ac::LocalProvider());
}

void ac_add_asset_source(ac_api_provider* local_provider, ac_asset_source* asset_source, int priority) {
    auto lp = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(lp);
    ac::asset::AssetSource* as = reinterpret_cast<ac::asset::AssetSource*>(asset_source);
    lp->addAssetSource(std::unique_ptr<ac::asset::AssetSource>(as), priority);
}

void ac_add_model(ac_api_provider* local_provider,
    const char* model_id, const char* inference_type,
    ac_model_info_asset* assets, size_t assets_count
) {
    auto lp = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(lp);

    std::vector<ac::ModelInfo::Asset> assets_vec;
    assets_vec.reserve(assets_count);
    for (size_t i = 0; i < assets_count; ++i) {
        auto& info = assets_vec.emplace_back();
        assert(assets[i].id);
        info.id = assets[i].id;
        if (assets[i].tag) {
            info.tag = assets[i].tag;
        }
    }

    lp->addModel(ac::ModelInfo{
        .id = model_id,
        .inferenceType = inference_type,
        .assets = std::move(assets_vec)
    });
}

}
