// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/xec/coro.hpp>
#include <ac/xec/context.hpp>
#include <ac/xec/co_spawn.hpp>
#include <doctest/doctest.h>

// GCC does not implement symmetric transfer with O0
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100897
// https://github.com/iboB/gcc-coro-stack-overflow

#if !defined(__GNUC__)
// enable if not gnuc
#define ENABLE 1
#elif defined(__clang__)
// enable with clang
#define ENABLE 1
// enable with O1 or more
#elif defined(__OPTIMIZE__) && __OPTIMIZE__ > 0
#define ENABLE 1
#else
// disable with gcc and O0
#define ENABLE 0
#endif

#if ENABLE

#define B_STACKTRACE_IMPL
#include <b_stacktrace.h>

using namespace ac::xec;

coro<int> depth() {
    auto trace = b_stacktrace_get();
    auto ret = b_stacktrace_depth(trace);
    free(trace);
    co_return ret;
}

coro<void> stack_flatten_test() {
    // eagerly resuming on return does not increase the stack depth
    // it is expected to be flattened as per https://eel.is/c++draft/expr.await#note-1
    const auto d = co_await depth();
    CHECK(d == co_await depth());
    CHECK(d == co_await depth());
    CHECK(d == co_await depth());
}

TEST_CASE("stack flatten") {
    context ctx;
    co_spawn(ctx, stack_flatten_test());
    ctx.run();
}

#endif
