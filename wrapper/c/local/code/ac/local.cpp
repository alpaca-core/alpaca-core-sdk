// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local.h"

#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/ModelLoader.hpp>
#include <ac/local/ModelFactory.hpp>

#include <ac/DictCUtil.hpp>

#include <astl/move.hpp>

#include <string>

using namespace ac::cutil;
using namespace ac::local;

struct ac_local_model {
    ModelPtr model;
};

struct ac_local_model_factory {
    ModelFactory factory;
};

namespace ac::local::cutil {
ModelFactory& ModelFactory_toCpp(ac_local_model_factory* f) {
    return f->factory;
}
}

namespace {
thread_local std::string local_last_error;

template <typename F>
auto local_try_catch(F&& f) noexcept -> decltype(f()) {
    local_last_error.clear();
    try {
        return f();
    }
    catch (const std::exception& e) {
        local_last_error = e.what();
    }
    catch (...) {
        local_last_error = "Unknown error";
    }
    return {};
}

inline Instance* Instance_toCpp(ac_local_instance* p) {
    return reinterpret_cast<Instance*>(p);
}
inline ac_local_instance* Instance_fromCpp(Instance* p) {
    return reinterpret_cast<ac_local_instance*>(p);
}
} // namespace

extern "C" {

const char* ac_local_get_last_error() {
    if (local_last_error.empty()) return nullptr;
    return local_last_error.c_str();
}

void ac_free_local_instance(ac_local_instance* i) {
    delete Instance_toCpp(i);
}

ac_dict_root* ac_run_local_op(
    ac_local_instance* i,
    const char* op,
    ac_dict_root* params_root,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
) {
    return local_try_catch([&] {
        auto& instance = *Instance_toCpp(i);
        auto params = Dict_from_dict_root_consume(params_root);

        ProgressCb pcb;
        if (progress_cb) {
            pcb = [=](std::string_view tag, float progress) {
                return progress_cb(ac_sv::from_std(tag), progress, cb_user_data);
            };
        }

        auto result = instance.runOp(op, astl::move(params), astl::move(pcb));
        return Dict_to_dict_root(result);
    });
}

void ac_free_local_model(ac_local_model* m) {
    delete m;
}

ac_local_instance* ac_create_local_instance(
    ac_local_model* m,
    const char* instance_type,
    ac_dict_root* params_root
) {
    return local_try_catch([&] {
        auto instance = m->model->createInstance(instance_type, Dict_from_dict_root_consume(params_root));
        return Instance_fromCpp(instance.release());
    });
}

ac_local_model_factory* ac_new_local_model_factory() {
    return local_try_catch([&] {
        return new ac_local_model_factory;
    });
}

void ac_free_local_model_factory(ac_local_model_factory* f) {
    delete f;
}

ac_local_model* ac_create_local_model(
    ac_local_model_factory* f,
    const char* inference_type,
    ac_local_model_desc_asset* assets,
    size_t assets_count,
    ac_dict_root* params_root,
    bool (*progress_cb)(ac_sv tag, float progress, void* user_data),
    void* cb_user_data
) {
    return local_try_catch([&] {
        ModelDesc desc = {.inferenceType = inference_type};
        desc.assets.reserve(assets_count);
        for (size_t i = 0; i < assets_count; ++i) {
            auto& info = desc.assets.emplace_back();
            assert(assets[i].path);
            info.path = assets[i].path;
            if (assets[i].tag) {
                info.tag = assets[i].tag;
            }
        }

        ProgressCb pcb;
        if (progress_cb) {
            pcb = [=](std::string_view tag, float progress) {
                return progress_cb(ac_sv::from_std(tag), progress, cb_user_data);
            };
        }

        auto model = f->factory.createModel(astl::move(desc), Dict_from_dict_root_consume(params_root), astl::move(pcb));
        return new ac_local_model{astl::move(model)};
    });
}

} // extern "C"
