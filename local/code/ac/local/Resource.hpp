// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <chrono>
#include <string>

namespace ac::local {

// Resources are stored type erased in the ResourceManager.
// They are reified by static_pointer_cast to the correct type.
// That's why you need to inherit from Resource to use the ResourceManager.
struct Resource {
    using seconds_t = std::chrono::duration<int32_t>;
    seconds_t maxAge = seconds_t(10);

    std::string space = "generic";
protected:
    ~Resource() = default;
};

} // namespace ac::local
