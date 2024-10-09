// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AlpacaCore.hpp"

#include <ac/local/LocalDummy.hpp>
#include <ac/local/LocalLlama.hpp>
#include <ac/local/LocalWhisper.hpp>

#include "Model.hpp"

namespace ac {

void initSDK() {
    factorySingleton = std::make_unique<local::ModelFactory>();

    local::addDummyInference(*factorySingleton);
    local::addLlamaInference(*factorySingleton);
    local::addWhisperInference(*factorySingleton);
}

class Model* createModel(AlpacaCore::ModelDesc& desc, DictRef params, ProgressCb pcb) {
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

    return new Model(factorySingleton->createModel(modelDesc, params.getDict(), nullptr));
}

}
