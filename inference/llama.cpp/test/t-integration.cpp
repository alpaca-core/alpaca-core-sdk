// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/llama/Init.hpp>
#include <ac/llama/Model.hpp>

#include <doctest/doctest.h>

#include "ac-test-data-llama-dir.h"

struct GlobalFixture {
    GlobalFixture() {
        ac::llama::initLibrary();
    }
};

GlobalFixture globalFixture;

const char* Model_117m_q6_k = AC_TEST_DATA_LLAMA_DIR "/gpt2-117m-q6_k.gguf";

TEST_CASE("vocab only") {
    ac::llama::Model model(Model_117m_q6_k, {.vocabOnly = true});
    CHECK(!!model.lmodel());

    auto& params = model.params();
    CHECK(params.gpu);
    CHECK(params.vocabOnly);

    CHECK(model.trainCtxLength() == 0); // no weights - no training context
    CHECK_FALSE(model.shouldAddBosToken());
    CHECK_FALSE(model.hasEncoder());

    // vocab works
    auto& vocab = model.vocab();
    CHECK(vocab.tokenToString(443) == " le");
    CHECK(vocab.tokenize("hello world", true, true) == std::vector<ac::llama::Token>{31373, 995});
}

