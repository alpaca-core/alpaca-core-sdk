// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_dummy.h"
#include "LocalDummy.hpp"
#include <ac/LocalProvider.hpp>
#include <ac/ApiCUtil.hpp>
#include <cassert>

extern "C" void ac_add_local_dummy_inference(ac_api_provider* local_provider) {
    auto localProvider = dynamic_cast<ac::LocalProvider*>(ac::cutil::Provider_from_provider(local_provider));
    assert(localProvider);
    ac::addLocalDummyInference(*localProvider);
}
