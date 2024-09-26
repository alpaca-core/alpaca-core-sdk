// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelFactory.hpp>
#include <ac/local/ModelLoader.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <doctest/doctest.h>
#include <memory>

struct TestModel final : public ac::local::Model {
    std::unique_ptr<ac::local::Instance> createInstance(std::string_view, ac::Dict) override {
        return {};
    }
};

struct ModelLoaderA final : public ac::local::ModelLoader {
    ac::local::ModelPtr loadModel(ac::local::ModelDesc desc, ac::Dict dict, ac::local::ProgressCb cb) override {
        CHECK(desc.inferenceType == "a");
        if (!desc.name.starts_with("a")) throw std::runtime_error("bad a name");
        CHECK(desc.assets.empty());
        CHECK(dict.get<int>() == 42);
        CHECK(cb("a", 1.f));
        return std::make_unique<TestModel>();
    }
};

struct ModelLoaderB final : public ac::local::ModelLoader {
    ac::local::ModelPtr loadModel(ac::local::ModelDesc desc, ac::Dict dict, ac::local::ProgressCb cb) override {
        CHECK(desc.inferenceType == "b");
        if (!desc.name.starts_with("b")) throw std::runtime_error("bad b name");
        CHECK(desc.assets.size() == 3);
        CHECK(dict.get<int>() == 42);
        CHECK(cb("b", 0.f));
        return std::make_unique<TestModel>();
    }
};

struct Progress {
    std::string tag;
    float progress;
};

TEST_CASE("ModelFactory") {
    ac::local::ModelFactory factory;
    ModelLoaderA loaderA;
    ModelLoaderB loaderB;
    factory.addLoader("a", loaderA);

    ac::local::ModelDesc descA;
    descA.inferenceType = "a";
    descA.name = "a";
    ac::Dict dict = 42;

    std::string progressTag;
    float progress;
    auto progressFunc = [&](std::string_view t, float p) {
        progressTag = std::string(t);
        progress = p;
        return true;
    };

    CHECK(factory.createModel(descA, dict, progressFunc));
    CHECK(progressTag == "a");
    CHECK(progress == 1.f);

    descA.name = "bad";
    CHECK_THROWS_WITH_AS(factory.createModel(descA, {}, {}), "bad a name", std::runtime_error);

    ac::local::ModelDesc descB;
    descB.inferenceType = "b";
    descB.name = "b";
    descB.assets.resize(3);

    CHECK_THROWS_WITH_AS(factory.createModel(descB, {}, {}), "Unknown inference type: b", std::runtime_error);

    factory.addLoader("b", loaderB);

    CHECK(factory.createModel(descB, dict, progressFunc));
    CHECK(progressTag == "b");
    CHECK(progress == 0.f);

    factory.removeLoader("a");
    CHECK_THROWS_WITH_AS(factory.createModel(descA, {}, {}), "Unknown inference type: a", std::runtime_error);

    CHECK_THROWS_WITH_AS(factory.removeLoader("c"), "Loader not found: c", std::runtime_error);

    CHECK_THROWS_WITH_AS(factory.addLoader("b", loaderB), "Loader already exists: b", std::runtime_error);
}
