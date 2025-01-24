// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "StreamPtr.hpp"
#include "Stream.hpp"

namespace ac::frameio {

class BasicStreamIo {
public:
    BasicStreamIo(StreamPtr stream) : m_stream(std::move(stream)) {}
protected:
    ~BasicStreamIo() = default;
    void close() { m_stream->close(); }
    StreamPtr m_stream;
};

} // namespace ac::frameio
