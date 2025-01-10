// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ProviderRegistry.hpp>
#include <ac/local/Provider.hpp>
#include <ac/local/CommonProviderScorers.hpp>
#include <doctest/doctest.h>

using Info = ac::local::Provider::Info;

Info LlamaA{
    .name = "llama a",
};

Info LlamaB{
    .name = "llama b",
};

Info WhisperX{
    .name = "whisper x",
};

struct TestProvider : public ac::local::Provider {
    const Info& m_info;
    TestProvider(const Info& info) : m_info(info) {}
    virtual const Info& info() const noexcept override { return m_info; }
    virtual bool canLoadModel(const ac::local::ModelAssetDesc& desc, const ac::Dict&) const noexcept override {
        return m_info.name.starts_with(desc.type);
    }
    virtual ac::local::ModelPtr loadModel(ac::local::ModelAssetDesc, ac::Dict, ac::local::ProgressCb) override {
        return {};
    }
};

TEST_CASE("ProviderRegistry") {
    ac::local::ProviderRegistry registry;

    ac::local::ModelAssetDesc
        llama{ .type = "llama", .name = "llama-7b" },
        whisper{ .type = "whisper", .name = "whisper-tiny" };

    ac::local::CanLoadScorer s;
    CHECK_FALSE(registry.findBestProvider(s, llama, {}));

    TestProvider llamaA(LlamaA), llamaB(LlamaB), whisperX(WhisperX);

    registry.addProvider(llamaA);
    registry.addProvider(llamaB);
    registry.addProvider(whisperX);

    auto la = registry.findBestProvider(s, llama, {});
    REQUIRE(la);
    CHECK(&la->info() == &LlamaA);

    auto wh = registry.findBestProvider(s, whisper, {});
    REQUIRE(wh);
    CHECK(&wh->info() == &WhisperX);
}
