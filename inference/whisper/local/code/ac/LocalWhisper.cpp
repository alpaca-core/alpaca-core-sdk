// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalWhisper.hpp"

#include <ac/whisper/Instance.hpp>
#include <ac/whisper/Init.hpp>
#include <ac/whisper/Model.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/LocalInference.hpp>

#include <astl/move.hpp>
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac {

namespace {

class WhisperInstance final : public LocalInferenceInstance {
    whisper::Instance m_instance;
public:
    WhisperInstance(whisper::Model& model)
        : m_instance(model, {})
    {}

    void run(Dict params, std::function<void(Dict)> streamCb) {
        auto value = params.find("audioBinaryMono");
        assert(value->is_binary());

        auto pcmu8 = value->get_binary();
        auto pcmf32 = reinterpret_cast<float*>(pcmu8.data());
        auto pcmf32Size = pcmu8.size() / sizeof(float);

        m_instance.runOp("transcribe", std::span{pcmf32, pcmf32Size}, [&](std::string res){
            streamCb({{"result", astl::move(res)}});
        });
    }

    virtual void runOpSync(std::string_view op, Dict params, std::function<void(Dict)> streamCb) override {
        if (op == "run") {
            run(astl::move(params), astl::move(streamCb));
        }
        else {
            throw_ex{} << "whisper: unknown op: " << op;
        }
    }
};

class WhisperModel final : public LocalInferenceModel {
    whisper::Model m_model;
public:
    WhisperModel(const std::string& binPath, whisper::Model::Params params)
        : m_model(binPath.c_str(), params)
    {}

    virtual std::unique_ptr<LocalInferenceInstance> createInstanceSync(std::string_view type, Dict) override {
        if (type != "general") {
            throw_ex{} << "whisper: unknown instance type: " << type;
        }
        return std::make_unique<WhisperInstance>(m_model);
    }
};

class WhisperModelLoader final : public LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<LocalInferenceModel> loadModelSync(Dict params, std::function<void(float)>) override {
        auto binPath = params.at("bin").get<std::string>();
        whisper::Model::Params modelParams;
        return std::make_unique<WhisperModel>(binPath, modelParams);
    }
};
}

void addLocalWhisperInference(LocalProvider& provider) {
    ac::whisper::initLibrary();

    static WhisperModelLoader loader;
    provider.addLocalInferenceLoader("whisper.cpp", loader);
}

} // namespace ac
