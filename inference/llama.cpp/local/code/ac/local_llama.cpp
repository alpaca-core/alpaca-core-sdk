// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_llama.h"
#include "LocalLlama.hpp"
#include <ac/LocalProvider.hpp>
#include <ac/LocalProviderCUtil.hpp>
#include <cassert>

extern "C" void ac_add_local_llama_inference(ac_local_provider* local_provider) {
    auto localProvider = ac::cutil::LocalProvider_toCpp(local_provider);
    assert(localProvider);
    ac::addLocalLlamaInference(*localProvider);
}
