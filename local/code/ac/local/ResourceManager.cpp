// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ResourceManager.hpp"
#include "Logging.hpp"
#include <ac/xec/post.hpp>
#include <astl/qalgorithm.hpp>

namespace ac::local {
namespace impl {
void Resource_touch(Resource& resource) {
    resource.touchExpiry();
}

} // namespace impl

} // namespace ac::local
