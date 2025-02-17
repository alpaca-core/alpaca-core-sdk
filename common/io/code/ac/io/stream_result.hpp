// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "value_with_status.hpp"
#include <ac/xec/task.hpp>

namespace ac::io {
using stream_result = value_with_status<xec::task>;
} // namespace ac::io
