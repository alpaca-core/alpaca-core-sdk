// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "local_provider.h"
#include "LocalProvider.hpp"
#include <ac/ApiCUtil.hpp>

extern "C" ac_api_provider* ac_new_local_api_provider() {
    return ac::cutil::Provider_to_provider(new ac::LocalProvider());
}
