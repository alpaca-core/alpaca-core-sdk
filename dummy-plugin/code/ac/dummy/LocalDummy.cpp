// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalDummy.hpp"
#include "DummySchema.hpp"

#include "Instance.hpp"
#include "Model.hpp"

#include "aclp-dummy-version.h"

#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/ModelLoader.hpp>

#include <ac/schema/DispatchHelpers.hpp>

#include <astl/move.hpp>
#include <astl/iile.h>
#include <astl/throw_stdex.hpp>

namespace ac::local {

namespace {

class DummyInstance final : public Instance {
    std::shared_ptr<dummy::Model> m_model;
    dummy::Instance m_instance;
public:
    using Schema = schema::Dummy::InstanceGeneral;

    static dummy::Instance::InitParams InitParams_fromDict(Dict&& d) {
        auto schemaParams = schema::Struct_fromDict<Schema::Params>(astl::move(d));
        dummy::Instance::InitParams ret;
        ret.cutoff = schemaParams.cutoff;
        return ret;
    }

    DummyInstance(std::shared_ptr<dummy::Model> model, Dict&& params)
        : m_model(astl::move(model))
        , m_instance(*m_model, InitParams_fromDict(astl::move(params)))
    {}

    Dict run(Dict& params) {
        const auto schemaParams = schema::Struct_fromDict<schema::DummyAInterface::OpRun::Params>(params);

        dummy::Instance::SessionParams sparams;
        sparams.splice = schemaParams.splice;
        sparams.throwOn = schemaParams.throwOn;

        auto s = m_instance.newSession(std::move(schemaParams.input), sparams);

        schema::DummyAInterface::OpRun::Return ret;
        auto res = ret.result.materialize();
        for (auto& w : s) {
            res += w;
            res += ' ';
        }
        if (!res.empty()) {
            // remove last space
            res.pop_back();
        }

        return schema::Struct_toDict(std::move(ret));
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        schema::dispatchById<schema::DummyAInterface::Ops>(op, std::move(params), *this);
    }
};

class DummyModel final : public Model {
    std::shared_ptr<dummy::Model> m_model;
public:
    using Schema = schema::Dummy;

    static dummy::Model::Params ModelParams_fromDict(Dict& d) {
        auto schemaParams = Schema::Params::fromDict(d);
        dummy::Model::Params ret;
        ret.splice = astl::move(schemaParams.spliceString.value_or(""));
        return ret;
    }

    DummyModel(const std::string& fname, Dict& params)
        : m_model(std::make_shared<dummy::Model>(fname.c_str(), ModelParams_fromDict(params)))
    {}
    explicit DummyModel(Dict& params) : m_model(std::make_shared<dummy::Model>(ModelParams_fromDict(params))) {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict params) override {
        switch (Schema::getInstanceById(type)) {
        case Schema::instanceIndex<Schema::InstanceGeneral>:
            return std::make_unique<DummyInstance>(m_model, params);
        default:
            throw_ex{} << "dummy: unknown instance type: " << type;
            MSVC_WO_10766806();
        }
    }
};

class DummyModelLoader final : public ModelLoader {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac-local dummy",
            .vendor = "Alpaca Core",
        };
        return i;
    }

    virtual bool canLoadModel(const ModelAssetDesc& desc, const Dict&) const noexcept override {
        return desc.type == "dummy";
    }

    virtual ModelPtr loadModel(ModelAssetDesc desc, Dict params, ProgressCb pcb) override {
        if (desc.assets.size() > 1) throw_ex{} << "dummy: expected one or zero assets";

        if (desc.assets.empty()) {
            // synthetic model
            if (pcb) {
                if (!pcb("synthetic", 0.5f)) {
                    throw_ex{} << "dummy: loading model aborted";
                }
            }
            return std::make_shared<DummyModel>(params);
        }
        else {
            auto& fname = desc.assets.front().path;
            if (pcb) {
                if (!pcb(fname, 0.1f)) {
                    throw_ex{} << "dummy: loading model aborted";
                }
            }
            return std::make_shared<DummyModel>(fname, params);
        }
    }
};

} // namespace

} // namespace ac::local

namespace ac::dummy {

std::vector<ac::local::ModelLoaderPtr> getLoaders() {
    std::vector<ac::local::ModelLoaderPtr> ret;
    ret.push_back(std::make_unique<local::DummyModelLoader>());
    return ret;
}

local::PluginInterface getPluginInterface() {
    return {
        .label = "ac-local dummy",
        .desc = "Dummy plugin for ac-local",
        .vendor = "Alpaca Core",
        .version = astl::version{
            ACLP_dummy_VERSION_MAJOR, ACLP_dummy_VERSION_MINOR, ACLP_dummy_VERSION_PATCH
        },
        .init = nullptr,
        .getLoaders = getLoaders,
    };
}

} // namespace ac::dummy
