// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <chrono>
#include <string>

namespace ac::local {
class ResourceManager;

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
    friend class ResourceManager;
    using time_point_t = std::chrono::steady_clock::time_point;
    time_point_t expireTime = time_point_t::min();
};

} // namespace ac::local
