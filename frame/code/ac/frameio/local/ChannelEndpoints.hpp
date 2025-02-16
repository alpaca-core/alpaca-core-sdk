// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../StreamEndpoint.hpp"

namespace ac::frameio {
struct ChannelEndpoints {
    StreamEndpoint ab;
    StreamEndpoint ba;
};
} // namespace ac::frameio
