// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_whisper.h"
#include "LocalWhisper.hpp"
#include <ac/LocalProvider.hpp>
#include <ac/LocalProviderCUtil.hpp>
#include <cassert>

extern "C" void ac_add_local_whisper_inference(ac_local_provider* local_provider) {
    auto localProvider = ac::cutil::LocalProvider_toCpp(local_provider);
    assert(localProvider);
    ac::addLocalWhisperInference(*localProvider);
}
