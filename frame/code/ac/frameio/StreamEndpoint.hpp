// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "StreamPtr.hpp"
#include "Stream.hpp"

namespace ac::frameio {

struct StreamEndpoint {
    ReadStreamPtr readStream;
    WriteStreamPtr writeStream;
};

} // namespace ac::frameio
