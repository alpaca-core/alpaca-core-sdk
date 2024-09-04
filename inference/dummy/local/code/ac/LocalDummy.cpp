// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"

#include <ac/dummy/Instance.hpp>
#include <ac/dummy/Model.hpp>

#include <ac/LocalProvider.hpp>
#include <ac/LocalModelInfo.hpp>
#include <ac/LocalInference.hpp>

#include <astl/move.hpp>
#include <astl/iile.h>
#include <astl/throw_ex.hpp>

namespace ac {

namespace {
dummy::Model::Params ModelParams_fromDict(const Dict& d) {
    dummy::Model::Params ret;
    Dict_optApplyValueAt(d, "splice", ret.splice);
    return ret;
}

dummy::Instance::InitParams InitParams_fromDict(const Dict& d) {
    dummy::Instance::InitParams ret;
    Dict_optApplyValueAt(d, "cutoff", ret.cutoff);
    return ret;
}

dummy::Instance::SessionParams SessionParams_fromDict(const Dict& d) {
    dummy::Instance::SessionParams ret;
    Dict_optApplyValueAt(d, "splice", ret.splice);
    Dict_optApplyValueAt(d, "throw_on", ret.throwOn);

    return ret;
}

class DummyInstance final : public LocalInferenceInstance {
    dummy::Instance m_instance;
public:
    DummyInstance(dummy::Model& model, dummy::Instance::InitParams params)
        : m_instance(model, astl::move(params))
    {}

    void run(Dict params, std::function<void(Dict)> streamCb) {
        auto input = params.at("input").get<std::vector<std::string>>();
        auto sparams = SessionParams_fromDict(params);

        auto s = m_instance.newSession(astl::move(input), sparams);

        std::string result;
        for (auto& w : s) {
            result += w;
            result += ' ';
        }
        if (!result.empty()) {
            // remove last space
            result.pop_back();
        }
        streamCb({{"result", astl::move(result)}});
    }

    virtual void runOpSync(std::string_view op, Dict params, std::function<void(Dict)> streamCb) override {
        if (op == "run") {
            run(astl::move(params), astl::move(streamCb));
        }
        else {
            throw_ex{} << "dummy: unknown op: " << op;
        }
    }
};

class DummyModel final : public LocalInferenceModel {
    dummy::Model m_model;
public:
    DummyModel(const std::string& fname, dummy::Model::Params params)
        : m_model(fname.c_str(), params)
    {}

    virtual std::unique_ptr<LocalInferenceInstance> createInstanceSync(std::string_view type, Dict params) override {
        if (type != "general") {
            throw_ex{} << "dummy: unknown instance type: " << type;
        }
        return std::make_unique<DummyInstance>(m_model, InitParams_fromDict(params));
    }
};

class DummyModelLoader final : public LocalInferenceModelLoader {
public:
    virtual std::unique_ptr<LocalInferenceModel> loadModelSync(LocalModelInfoPtr info, Dict params, std::function<void(float)>) override {
        if (!info) throw_ex{} << "dummy: no model info";
        if (info->localAssets.size() != 1) throw_ex{} << "dummy: expected exactly one local asset";
        auto& fname = info->localAssets.front().path;
        if (!fname) throw_ex{} << "dummy: missing fname path";
        dummy::Model::Params modelParams;
        return std::make_unique<DummyModel>(*fname, modelParams);
    }
};
}

void addLocalDummyInference(LocalProvider& provider) {
    static DummyModelLoader loader;
    provider.addLocalInferenceLoader("dummy", loader);
}

} // namespace ac
