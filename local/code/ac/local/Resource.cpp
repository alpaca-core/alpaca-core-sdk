// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Resource.hpp"

namespace ac::local::impl {
void Resource_touch(Resource& resource) {
    resource.expireTime = std::chrono::steady_clock::now() + resource.maxAge;
}
} // namespace ac::local::impl
