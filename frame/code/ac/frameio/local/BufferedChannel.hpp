// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../../Frame.hpp"
#include <ac/io/buffered_channel.hpp>
#include <memory>

namespace ac::frameio {

using BufferedChannel = ac::io::buffered_channel<Frame>;

inline std::unique_ptr<BufferedChannel> BufferedChannel_create(size_t maxSize) {
    return std::make_unique<BufferedChannel>(maxSize);
}

} // namespace ac::frameio
