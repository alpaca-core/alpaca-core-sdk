// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.hpp"

// a dummy local provider used in tests only

#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/ApiCUtil.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/local_provider.h>
#include <ac/LocalProviderCUtil.hpp>

#include <astl/throw_ex.hpp>

#include <thread>

namespace {

class DummyLocalInferenceInstance final : public ac::LocalInferenceInstance {
public:
    virtual void runOpSync(std::string_view op, ac::Dict params, std::function<void(ac::Dict)> streamCb, ac::ProgressCb pcb) override {
        pcb("stream", 0.1f);

        if (op == "insta") {
            streamCb({{"insta", "success"}});
            return;
        }

        pcb("stream", 0.5f);

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
    ac::ModelDesc desc,
    ac::Dict params,
    ac::ProgressCb progress
) {
    auto& assets = desc.assets;
    if (assets.size() != 2) {
        ac::throw_ex{} << "expected 2 assets, got " << assets.size();
    }
    progress("dummy model", 0.2f);
    for (auto& a : assets) {
        if (a.tag == "error") {
            ac::throw_ex{} << "asset error: " << a.path;
        }
    }
    progress("dummy model", 0.4f);

    progress("dummy model", 0.5f);

    if (ac::Dict_optValueAt(params, "error", false)) {
        return nullptr;
    }

    progress("dummy model", 1.f);
    return std::unique_ptr<DummyLocalInferenceModel>(new DummyLocalInferenceModel());
}

extern "C" uint64_t get_thread_id() {
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
}

extern "C" void add_dummy_inference(ac_local_provider* local_provider) {
    static DummyLocalInferenceModelLoader loader;
    auto localProvider = ac::cutil::LocalProvider_toCpp(local_provider);
    localProvider->addLocalInferenceLoader("dummy", loader);
}
