// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Stream.hpp"
namespace ac::rpc {
// only export vtable
Stream::~Stream() = default;
ReadStream::~ReadStream() = default;
WriteStream::~WriteStream() = default;
} // namespace ac::rpc
