// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <jalog/Log.hpp>
#include <jalog/Scope.hpp>

namespace {

void log_default(int n) {
    JALOG(Error, "message from c++ ", n);
}

jalog::Scope cpp_scope("cpp_scope");

void log_scope(int n) {
    JALOG_SCOPE(cpp_scope, Critical, "scoped from c++ ", n);
}

} // namespace

extern "C" void run_cpp_logs(int n) {
    log_default(n);
    log_scope(n);
}
