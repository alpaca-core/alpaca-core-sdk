// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "value_with_status.hpp"

namespace ac::xec {
class notifiable;
}

namespace ac::io {
using stream_result = value_with_status<xec::notifiable*>;
} // namespace ac::io
