// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <ac/jalog/LogStream.hpp>

int main()
{
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    auto log = AC_JALOG_STREAM(Info);

    log << "Hello";
    log << ", stream world!" << ac::jalog::endl;

    log << ac::jalog::Level::Debug << "ac::jalog::Stream was used";

    return 0;
}
