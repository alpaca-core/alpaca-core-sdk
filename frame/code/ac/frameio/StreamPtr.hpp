// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <memory>

namespace ac::frameio {

class ReadStream;
using ReadStreamPtr = std::unique_ptr<ReadStream>;

class WriteStream;
using WriteStreamPtr = std::unique_ptr<WriteStream>;

} // namespace ac::frameio
