// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Log.hpp>
#include <ac/jalog/Scope.hpp>

namespace {

void log_default(int n) {
    AC_JALOG(Error, "message from c++ ", n);
}

ac::jalog::Scope cpp_scope("cpp_scope");

void log_scope(int n) {
    AC_JALOG_SCOPE(cpp_scope, Critical, "scoped from c++ ", n);
}

} // namespace

extern "C" void run_cpp_logs(int n) {
    log_default(n);
    log_scope(n);
}
