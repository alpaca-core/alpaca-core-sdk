// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <chrono>
#include <string>

namespace ac::local {

struct Resource;
namespace impl {
AC_LOCAL_EXPORT void Resource_touch(Resource& resource);
} // namespace impl

// Resources are stored type erased in the ResourceManager.
// They are reified by static_pointer_cast to the correct type.
// That's why you need to inherit from Resource to use the ResourceManager.
struct Resource {
    using seconds_t = std::chrono::duration<int32_t>;
    seconds_t maxAge = seconds_t(10);

    // not used yet
    // see https://github.com/alpaca-core/alpaca-core-sdk/discussions/250
    std::string space = "generic";
protected:

    ~Resource() = default;

private:

    void touchExpiry() {
        expireTime = std::chrono::steady_clock::now() + maxAge;
    }

    friend void impl::Resource_touch(Resource& resource);
    using time_point_t = std::chrono::steady_clock::time_point;
    time_point_t expireTime = time_point_t::min();
};

} // namespace ac::local
