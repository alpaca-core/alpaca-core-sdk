// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/LocalInference.hpp>

class DummyLocalInferenceModelLoader final : public ac::LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<ac::LocalInferenceModel> loadModelSync(
        ac::ModelDesc desc,
        ac::Dict params,
        ac::ProgressCb progress
    ) override;
};