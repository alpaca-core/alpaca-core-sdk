// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelLoaderRegistry.hpp>
#include <ac/local/ModelLoader.hpp>
#include <doctest/doctest.h>

using Info = ac::local::ModelLoader::Info;

Info LlamaA{
    .name = "llama a",
    .assetTypes = {"llama"},
};

Info LlamaB{
    .name = "llama b",
    .assetTypes = {"llama"},
};

Info Whisper{
    .name = "my whisper",
    .assetTypes = {"whisper"},
};

struct TestLoader : public ac::local::ModelLoader {
    const Info& m_info;
    TestLoader(const Info& info) : m_info(info) {}
    virtual const Info& info() const noexcept override { return m_info; }
    virtual bool canLoadModel(const ac::local::ModelAssetDesc&, const ac::Dict&) const noexcept override { return true; }
    virtual ac::local::ModelPtr loadModel(ac::local::ModelAssetDesc, ac::Dict, ac::local::ProgressCb) override {
        return {};
    }
};

TEST_CASE("ModelLoaderRegistry") {
    ac::local::ModelLoaderRegistry registry;
    CHECK_FALSE(registry.findLoader("llama"));

    TestLoader llamaA(LlamaA), llamaB(LlamaB), whisper(Whisper);

    registry.addLoader(llamaA);
    registry.addLoader(llamaB);
    registry.addLoader(whisper);

    auto la = registry.findLoader("llama");
    CHECK(!!la);
    CHECK(&la->loader.info() == &LlamaA);
    CHECK_FALSE(la->plugin);

    auto wh = registry.findLoader("whisper");
    CHECK(!!wh);
    CHECK(&wh->loader.info() == &Whisper);
}
