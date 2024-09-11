// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.hpp"

// a dummy local provider used in tests only

#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/ApiCUtil.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/LocalModelInfo.hpp>

#include <astl/throw_ex.hpp>

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

class DummyAssetSource final : public ac::asset::AssetSource {
public:
    virtual std::string_view id() const noexcept override { return "dummy-asset-source"; }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override {
        if (id == "asset1") {
            return BasicAssetInfo{1024, "/home/asset1"};
        }
        if (id == "asset2") {
            return BasicAssetInfo{{}, {}};
        }
        if (id == "bad-remote-asset") {
            return BasicAssetInfo{{}, {}};
        }
        return std::nullopt;
    }

    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb progressCb) override {
        if (id == "asset2") {
            progressCb(0.2f);
            progressCb(0.5f);
            progressCb(1.f);
            return BasicAssetInfo{512, "/home/asset2"};
        }
        else if (id == "bad-remote-asset") {
            progressCb(0.2f);
            throw std::runtime_error("Bad remote asset");
        }
        // shouldn't get called for asset1
        throw std::runtime_error("Can't get asset");
    }
};

} // anonymous namespace

std::unique_ptr<ac::LocalInferenceModel> DummyLocalInferenceModelLoader::loadModelSync(
    ac::LocalModelInfoPtr info,
    ac::Dict params,
    std::function<void(float)> progress
) {
    auto& assets = info->localAssets;
    if (assets.size() != 2) {
        ac::throw_ex{} << "expected 2 assets, got " << assets.size();
    }
    progress(0.2f);
    for (auto& a : assets) {
        if (a.error) {
            ac::throw_ex{} << "asset error: " << a.error.value();
        }
    }
    progress(0.4f);

    progress(0.5f);

    if (ac::Dict_optValueAt(params, "error", false)) {
        return nullptr;
    }

    progress(1.f);
    return std::unique_ptr<DummyLocalInferenceModel>(new DummyLocalInferenceModel());
}

std::unique_ptr<ac::asset::AssetSource> createDummyAssetSource() {
    return std::make_unique<DummyAssetSource>();
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

    localProvider->addAssetSource(createDummyAssetSource(), 0);

    localProvider->addModel(ac::ModelInfo{"empty"});
    localProvider->addModel(ac::ModelInfo{
        .id = "model",
        .inferenceType = "dummy",
        .assets = {
            {"asset1", "tag1"},
            {"asset2", "tag2"}
        }
    });
}
