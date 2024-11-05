// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AlpacaCore.hpp"
#include "Model.hpp"

#include <ac/local/ModelFactory.hpp>
#include <ac/local/LocalDummy.hpp>
#include <ac/local/LocalLlama.hpp>
#include <ac/local/LocalWhisper.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

namespace AC {

static std::unique_ptr<local::ModelFactory> factorySingleton;

void initSDK() {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    factorySingleton = std::make_unique<local::ModelFactory>();

    local::addDummyInference(*factorySingleton);
    local::addLlamaInference(*factorySingleton);
    local::addWhisperInference(*factorySingleton);
}

Expected<Model, std::string> createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData) {
    ac::local::ModelDesc modelDesc;
    modelDesc.inferenceType = desc.getInferenceType();
    modelDesc.name = desc.getName();

    for (auto asset : desc.getAssets()) {
        ac::local::ModelDesc::AssetInfo assetInfo;
        assetInfo.path = asset.getPath();
        assetInfo.tag = asset.getTag();
        modelDesc.assets.push_back(assetInfo);
    }

    try {
        if (progressCbData.m_cb) {
            return Model(factorySingleton->createModel(modelDesc, params.getDict(), [&](std::string_view tag, float progress) {
                progressCbData.m_cb(progressCbData.m_context, tag.data(), tag.data() + tag.size(), progress);
                return true;
            }));
        }
        else {
            return Model(factorySingleton->createModel(modelDesc, params.getDict()));
        }
    }
    catch(const std::exception& e)
    {
        return itlib::unexpected<std::string>(e.what());
    }
}

}
