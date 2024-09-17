// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_provider.h"
#include "LocalProvider.hpp"
#include "LocalProviderCUtil.hpp"
#include "ModelInfo.hpp"
#include <ac/asset/Source.hpp>
#include <ac/ApiCUtil.hpp>
#include <ac/DictCUtil.hpp>
#include <astl/move.hpp>

using namespace ac::cutil;

extern "C" {

ac_local_provider* ac_new_local_api_provider() {
    return LocalProvider_fromCpp(new ac::LocalProvider);;
}

void ac_free_local_provider(ac_local_provider* p) {
    delete LocalProvider_toCpp(p);;
}

void ac_add_asset_source(ac_local_provider* local_provider, ac_asset_source* asset_source, int priority) {
    auto lp = LocalProvider_toCpp(local_provider);
    ac::asset::Source* as = reinterpret_cast<ac::asset::Source*>(asset_source);
    lp->addAssetSource(std::unique_ptr<ac::asset::Source>(as), priority);
}

void ac_add_model(ac_local_provider* local_provider,
    const char* model_id, const char* inference_type,
    ac_model_info_asset* assets, size_t assets_count
) {
    auto lp = LocalProvider_toCpp(local_provider);

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

void ac_create_model(
    ac_local_provider* p,
    const char* model_id,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
) {
    auto provider = LocalProvider_toCpp(p);
    provider->createModel(model_id, Dict_from_dict_root_consume(dict_root), {
        [=](ac::CallbackResult<ac::ModelPtr> result) {
            if (result.has_value()) {
                result_cb(new ac_model{astl::move(result.value())}, nullptr, cb_user_data);
            }
            else {
                result_cb(nullptr, result.error().text.c_str(), cb_user_data);
            }
        },
        [=](std::string_view tag, float progress) {
            if (progress_cb) {
                progress_cb(ac_sv::from_std(tag), progress, cb_user_data);
            }
        }
    });
}

}
