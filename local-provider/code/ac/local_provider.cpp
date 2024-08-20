// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_provider.h"
#include "LocalProvider.hpp"
#include <ac/ApiCUtil.hpp>
#include <ac/DictCUtil.hpp>

extern "C" {

ac_api_provider* ac_new_local_api_provider() {
    return ac::cutil::Provider_to_provider(new ac::LocalProvider());
}

void ac_add_local_model(ac_api_provider* local_provider, const char* model_id, ac_dict_root* dict) {
    auto lp = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(lp);
    lp->addLocalModel(model_id, ac::cutil::Dict_from_dict_root_consume(dict));
}

}
