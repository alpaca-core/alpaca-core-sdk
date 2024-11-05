// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <ac/jalog/DefaultScope.hpp>

#include <sstream>

int main()
{
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    std::ostringstream sout;
    sout << "Producing a custom string " << 34 << ' ' << 4.2;
    ac::jalog::Default_Scope.addEntry(ac::jalog::Level::Info, sout.str());

    return 0;
}
