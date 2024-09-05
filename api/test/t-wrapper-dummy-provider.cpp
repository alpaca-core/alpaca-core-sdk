// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// a dummy synchronous provider used in tests only

#include <ac/Provider.hpp>
#include <ac/Model.hpp>
#include <ac/Instance.hpp>
#include <ac/ApiCUtil.hpp>

namespace {
class DummyInstance final : public ac::Instance {
    void runOp(std::string_view op, ac::Dict params, ac::Callback<void, ac::Dict> cb) override {
        std::string_view tag = "stream";

        if (op == "insta") {
            cb.progressCb(tag, {{"insta", "success"}});
            cb.resultCb({});
            return;
        }

        cb.progressCb(tag, {{"some", 42}});

        if (op == "error") {
            cb.resultCb(itlib::unexpected(ac::Error{params.at("error").get<std::string>()}));
        }
        else {
            cb.progressCb(tag, {{"more", 1024}});
            cb.resultCb({});
        }
    }

    void synchronize() override {} // not really testable

    void initiateAbort(ac::Callback<void> cb) override {
        cb.resultCb({});
    }
};

class DummyModel final : public ac::Model {
    void createInstance(std::string_view type, ac::Dict params, ac::Callback<ac::InstancePtr> cb) override {
        cb.progressCb(type, 0.1f);
        if (type == "error") {
            cb.resultCb(itlib::unexpected(ac::Error{params.at("error").get<std::string>()}));
            return;
        }

        cb.progressCb(type, 1.f);
        cb.resultCb(ac::InstancePtr{std::make_shared<DummyInstance>()});
    }
};

class DummyProvider final : public ac::Provider {
    void createModel(std::string_view id, ac::Dict params, ac::Callback<ac::ModelPtr> cb) override {
        cb.progressCb(id, 0.2f);

        if (id == "error") {
            cb.resultCb(itlib::unexpected(ac::Error{"dummy id error"}));
            return;
        }

        cb.progressCb(id, 0.5f);

        if (ac::Dict_optValueAt(params, "error", false)) {
            cb.resultCb(itlib::unexpected(ac::Error{"dummy param error"}));
            return;
        }

        cb.progressCb(id, 1.f);
        cb.resultCb(ac::ModelPtr{std::make_shared<DummyModel>()});
    }
};

} // anonymous namespace

extern "C" ac_api_provider* create_dummy_provider() {
    return ac::cutil::Provider_to_provider(new DummyProvider);
}
