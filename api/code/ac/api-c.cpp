// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "api.h"
#include "dict.h"
#include "DictCUtil.hpp"
#include "ApiCUtil.hpp"
#include "Instance.hpp"
#include "Model.hpp"
#include "Provider.hpp"

#include <astl/move.hpp>

#include <cassert>

namespace ac::cutil {
Provider* Provider_from_provider(ac_api_provider* p) {
    return reinterpret_cast<Provider*>(p);
}
ac_api_provider* Provider_to_provider(Provider* p) {
    return reinterpret_cast<ac_api_provider*>(p);
}
}

using namespace ac::cutil;

struct ac_model {
    ac::ModelPtr model;
};

struct ac_instance {
    ac::InstancePtr instance;
};

extern "C" {

void ac_free_api_provider(ac_api_provider* p) {
    auto provider = Provider_from_provider(p);
    delete provider;
}

void ac_free_model(ac_model* m) {
    delete m;
}

void ac_create_model_json_params(
    ac_api_provider* p,
    const char* model_id,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_model* m, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
) {
    auto provider = Provider_from_provider(p);
    provider->createModel(model_id, Dict_from_dict_root_consume(dict_root), {
        [=](ac::CallbackResult<ac::ModelPtr> result) {
            if (result.has_value()) {
                result_cb(new ac_model{astl::move(result.value())}, nullptr, cb_user_data);
            }
            else {
                result_cb(nullptr, result.error().text.c_str(), cb_user_data);
            }
        },
        [=](float progress) {
            if (progress_cb) {
                progress_cb(progress, cb_user_data);
            }
        }
    });
}

void ac_free_instance(ac_instance* i) {
    delete i;
}

void ac_create_instance_json_params(
    ac_model* m,
    const char* instance_type,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_instance* i, const char* error, void* user_data),
    void (*progress_cb)(float progress, void* user_data),
    void* cb_user_data
) {
    m->model->createInstance(instance_type, Dict_from_dict_root_consume(dict_root), {
        [=](ac::CallbackResult<ac::InstancePtr> result) {
            if (result.has_value()) {
                result_cb(new ac_instance{astl::move(result.value())}, nullptr, cb_user_data);
            }
            else {
                result_cb(nullptr, result.error().text.c_str(), cb_user_data);
            }
        },
        [=](float progress) {
            if (progress_cb) {
                progress_cb(progress, cb_user_data);
            }
        }
    });
}

void ac_run_op_json_params(
    ac_instance* i,
    const char* op,
    ac_dict_root* dict_root,
    void (*result_cb)(const char* error, void* user_data),
    void (*stream_cb)(ac_dict_ref dict, void* user_data),
    void* cb_user_data
) {
    i->instance->runOp(op, Dict_from_dict_root_consume(dict_root), {
        [=](ac::CallbackResult<void> result) {
            if (result.has_value()) {
                result_cb(nullptr, cb_user_data);
            }
            else {
                result_cb(result.error().text.c_str(), cb_user_data);
            }
        },
        [=](ac::Dict dict) {
            if (stream_cb) {
                stream_cb(Dict_to_dict_ref(dict), cb_user_data);
            }
        }
    });
}

void ac_synchronize_instance(ac_instance* i) {
    i->instance->synchronize();
}

void ac_initiate_instance_abort(
    ac_instance* i,
    void (*done_cb)(const char* error, void* user_data),
    void* cb_user_data
) {
    i->instance->initiateAbort({
        [=](ac::CallbackResult<void> result) {
            if (result.has_value()) {
                done_cb(nullptr, cb_user_data);
            }
            else {
                done_cb(result.error().text.c_str(), cb_user_data);
            }
        },
        {}
    });
}

} // extern "C"