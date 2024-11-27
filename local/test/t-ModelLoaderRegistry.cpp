// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelLoaderRegistry.hpp>
#include <ac/local/ModelLoader.hpp>
#include <ac/local/CommonModelLoaderScorers.hpp>
#include <doctest/doctest.h>

using Info = ac::local::ModelLoader::Info;

Info LlamaA{
    .name = "llama a",
};

Info LlamaB{
    .name = "llama b",
};

Info WhisperX{
    .name = "whisper x",
};

struct TestLoader : public ac::local::ModelLoader {
    const Info& m_info;
    TestLoader(const Info& info) : m_info(info) {}
    virtual const Info& info() const noexcept override { return m_info; }
    virtual bool canLoadModel(const ac::local::ModelAssetDesc& desc, const ac::Dict&) const noexcept override {
        return m_info.name.starts_with(desc.type);
    }
    virtual ac::local::ModelPtr loadModel(ac::local::ModelAssetDesc, ac::Dict, ac::local::ProgressCb) override {
        return {};
    }
};

TEST_CASE("ModelLoaderRegistry") {
    ac::local::ModelLoaderRegistry registry;

    ac::local::ModelAssetDesc
        llama {.type = "llama", .name = "llama-7b"},
        whisper {.type = "whisper", .name = "whisper-tiny"};

    ac::local::CanLoadScorer s;
    CHECK_FALSE(registry.findBestLoader(s, llama, {}));

    TestLoader llamaA(LlamaA), llamaB(LlamaB), whisperX(WhisperX);

    registry.addLoader(llamaA);
    registry.addLoader(llamaB);
    registry.addLoader(whisperX);

    auto la = registry.findBestLoader(s, llama, {});
    REQUIRE(la);
    CHECK(&la->info() == &LlamaA);

    auto wh = registry.findBestLoader(s, whisper, {});
    REQUIRE(wh);
    CHECK(&wh->info() == &WhisperX);
}
