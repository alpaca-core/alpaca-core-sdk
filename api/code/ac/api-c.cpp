// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "api.h"
#include "dict.h"
#include "DictCUtil.hpp"
#include "ApiCUtil.hpp"
#include "Instance.hpp"
#include "Model.hpp"

#include <astl/move.hpp>

#include <cassert>

struct ac_instance {
    ac::InstancePtr instance;
};

using namespace ac::cutil;

extern "C" {

void ac_free_model(ac_model* m) {
    delete m;
}

void ac_free_instance(ac_instance* i) {
    delete i;
}

void ac_create_instance(
    ac_model* m,
    const char* instance_type,
    ac_dict_root* dict_root,
    void (*result_cb)(ac_instance* i, const char* error, void* user_data),
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
        }
    });
}

void ac_run_op(
    ac_instance* i,
    const char* op,
    ac_dict_root* dict_root,
    void (*completion_cb)(const char* error, void* user_data),
    void (*stream_cb)(ac_dict_ref dict, void* user_data),
    void (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
) {
    i->instance->runOp(op, Dict_from_dict_root_consume(dict_root), {
        [=](ac::CallbackResult<void> result) {
            if (result.has_value()) {
                completion_cb(nullptr, cb_user_data);
            }
            else {
                completion_cb(result.error().text.c_str(), cb_user_data);
            }
        },
        [=](ac::Dict dict) {
            stream_cb(Dict_to_dict_ref(dict), cb_user_data);
        },
        [=](std::string_view tag, float progress) {
            if (progress_cb) {
                progress_cb(ac_sv::from_std(tag), progress, cb_user_data);
            }
        }
    });
}

void ac_synchronize_instance(ac_instance* i) {
    i->instance->synchronize();
}

void ac_initiate_instance_abort(
    ac_instance* i,
    void (*done_cb)(void* user_data),
    void* cb_user_data
) {
    i->instance->initiateAbort([=]() {
        if (done_cb) {
            done_cb(cb_user_data);
        }
    });
}

} // extern "C"