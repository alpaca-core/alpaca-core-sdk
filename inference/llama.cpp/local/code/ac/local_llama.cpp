// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_llama.h"
#include "LocalLlama.hpp"
#include <ac/LocalProvider.hpp>
#include <ac/ApiCUtil.hpp>
#include <cassert>

extern "C" void ac_add_local_llama_inference(ac_api_provider* local_provider) {
    auto localProvider = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(localProvider);
    ac::addLocalLlamaInference(*localProvider);
}
