// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "Stream.hpp"
#include <ac/io/xio.hpp>
#include <ac/xec/timer_wobj.hpp>

namespace ac::frameio {

using Input = io::xinput<ReadStream, xec::timer_wobj>;
using Output = io::xoutput<WriteStream, xec::timer_wobj>;

} // namespace ac::frameio
