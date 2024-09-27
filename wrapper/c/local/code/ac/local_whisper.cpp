// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_whisper.h"
#include "local.h"
#include <ac/local/LocalWhisper.hpp>

extern "C" void ac_add_local_whisper_inference(struct ac_local_model_factory* factory) {
    ac::local::addWhisperInference(ac::local::cutil::ModelFactory_toCpp(factory));
}
