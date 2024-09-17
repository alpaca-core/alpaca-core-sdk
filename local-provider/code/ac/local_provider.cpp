// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_provider.h"
#include "LocalProvider.hpp"
#include "LocalProviderCUtil.hpp"
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
void ac_create_model(
    ac_local_provider* p,
    const char* inference_type,
    ac_model_desc_asset* assets,
    size_t assets_count,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
) {
    auto provider = LocalProvider_toCpp(p);

    ac::ModelDesc desc = {.inferenceType = inference_type};
    desc.assets.reserve(assets_count);
    for (size_t i = 0; i < assets_count; ++i) {
        auto& info = desc.assets.emplace_back();
        assert(assets[i].path);
        info.path = assets[i].path;
        if (assets[i].tag) {
            info.tag = assets[i].tag;
        }
    }

    provider->createModel(astl::move(desc), Dict_from_dict_root_consume(dict_root), {
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

} // extern "C"
