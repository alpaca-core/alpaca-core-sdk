// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.hpp"

// a dummy local provider used in tests only

#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/ApiCUtil.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/ModelInfo.hpp>

#include <thread>

namespace {

class DummyLocalInferenceInstance final : public ac::LocalInferenceInstance {
public:
    virtual void runOpSync(std::string_view op, ac::Dict params, std::function<void(ac::Dict)> streamCb) override {
        if (op == "insta") {
            streamCb({{"insta", "success"}});
            return;
        }

        streamCb({{"some", 42}});

        if (op == "error") {
            streamCb({{"error", params.at("error").get<std::string>() }});
            return;
        }
        else if (op == "more") {
            streamCb({{"more", 1024}});
        }
    }
};

class DummyLocalInferenceModel final : public ac::LocalInferenceModel {
public:
    virtual std::unique_ptr<ac::LocalInferenceInstance> createInstanceSync(std::string_view, ac::Dict params) override {

        if (ac::Dict_optValueAt(params, "error", false)) {
            return nullptr;
        }

        return std::unique_ptr<DummyLocalInferenceInstance>(new DummyLocalInferenceInstance());
    }
};

} // anonymous namespace

std::unique_ptr<ac::LocalInferenceModel> DummyLocalInferenceModelLoader::loadModelSync(
    ac::LocalModelInfoPtr info,
    ac::Dict params,
    std::function<void(float)> progress
) {
    progress(0.2f);
    progress(0.5f);

    if (ac::Dict_optValueAt(params, "error", false)) {
        return nullptr;
    }

    progress(1.f);
    return std::unique_ptr<DummyLocalInferenceModel>(new DummyLocalInferenceModel());
}

extern "C" uint64_t get_thread_id() {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}

extern "C" ac_api_provider* create_dummy_provider() {
    return ac::cutil::Provider_to_provider(new ac::LocalProvider);
}

extern "C" void add_dummy_inference(ac_api_provider* local_provider) {
    static DummyLocalInferenceModelLoader loader;
    auto localProvider = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(localProvider);
    localProvider->addLocalInferenceLoader("dummy", loader);

    localProvider->addModel(ac::ModelInfo{"empty"});
    localProvider->addModel(ac::ModelInfo{"model", "dummy"});
}
