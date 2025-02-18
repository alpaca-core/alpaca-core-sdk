// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Stream.hpp"
#include <ac/io/xio_endpoint.hpp>
#include <ac/xec/timer_wobj.hpp>

namespace ac::frameio {
using IoEndpoint = ac::io::xio_endpoint<ReadStream, WriteStream, xec::timer_wobj>;
} // namespace ac::frameio
