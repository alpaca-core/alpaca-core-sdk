// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AlpacaCore.hpp"

#include <ac/local/LocalDummy.hpp>
#include <ac/local/LocalLlama.hpp>
#include <ac/local/LocalWhisper.hpp>

#include <ac-audio.hpp>

#include "Model.hpp"

namespace ac {

void initSDK() {
    factorySingleton = std::make_unique<local::ModelFactory>();

    local::addDummyInference(*factorySingleton);
    local::addLlamaInference(*factorySingleton);
    local::addWhisperInference(*factorySingleton);
}

Model* createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData) {
    ac::local::ModelDesc modelDesc;
    modelDesc.inferenceType = desc.getInferenceType();
    modelDesc.name = desc.getName();

    for (auto asset : desc.getAssets())
    {
        ac::local::ModelDesc::AssetInfo assetInfo;
        assetInfo.path = asset.getPath();
        assetInfo.tag = asset.getTag();
        modelDesc.assets.push_back(assetInfo);
    }

    return new Model(factorySingleton->createModel(modelDesc, params.getDict(), [&](std::string_view tag, float progress) {
        progressCbData.m_cb(progressCbData.m_context, tag.data(), progress);
        return true;
    }));
}

std::vector<float> loadWavF32Mono(const std::string& path) {
    return ac::audio::loadWavF32Mono(path);
}

}
