// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "wait_func_concept.hpp"
#include <system_error>

namespace ac::xec {
template <wait_func_class Func>
void wait_func_invoke_timeout(Func& wf) {
    wf({});
}

template <wait_func_class Func>
void wait_func_invoke_cancelled(Func& wf) {
    wf(std::make_error_code(std::errc::operation_canceled));
}
} // namespace ac::xec
