// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once

#include <cstdint>

namespace ac::jalog
{

enum class Level : uint32_t
{
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

}
