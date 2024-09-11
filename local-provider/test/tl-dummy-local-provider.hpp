// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/LocalInference.hpp>
#include <ac/asset/Source.hpp>

class DummyLocalInferenceModelLoader final : public ac::LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<ac::LocalInferenceModel> loadModelSync(
        ac::LocalModelInfoPtr info,
        ac::Dict params,
        std::function<void(float)> progress
    ) override;
};

std::unique_ptr<ac::asset::AssetSource> createDummyAssetSource();