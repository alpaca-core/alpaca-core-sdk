// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelLoaderRegistry.hpp>
#include <doctest/doctest.h>

using Info = ac::local::ModelLoaderInfo;

Info makeLlamaA() {
    return Info{
        .name = "llama a",
        .schemaType = "llama",
    };
}

Info makeLlamaB() {
    return Info{
        .name = "llama b",
        .schemaType = "llama",
    };
}

Info makeWhisper() {
    return Info{
        .name = "my whisper",
        .schemaType = "whisper",
    };
}

TEST_CASE("ModelFactory") {
    ac::local::ModelLoaderRegistry registry;
    CHECK(registry.findLoader("llama") == nullptr);

    registry.loaders.push_back(makeLlamaA());
    registry.loaders.push_back(makeLlamaB());
    registry.loaders.push_back(makeWhisper());

    auto la = registry.findLoader("llama");
    CHECK(la != nullptr);
    CHECK(la->name == "llama a");

    auto wh = registry.findLoader("whisper");
    CHECK(wh != nullptr);
    CHECK(wh->name == "my whisper");
}
