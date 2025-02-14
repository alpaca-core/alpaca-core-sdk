// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <ac/jalog/Log.hpp>

int main()
{
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    AC_JALOG(Debug, "Perparing to greet world");
    AC_JALOG(Info, "Hello, world");

    return 0;
}
