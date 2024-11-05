// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>
#include <ac/jalog/Printf.hpp>

///////////////////////////////////////////////////////////////////////////////
// this is an imaginary c library
typedef void(*clib_log_func)(const char* fmt, ...);
clib_log_func clib_log_info;
clib_log_func clib_log_error;
void clib_do_x(int x)
{
    if (clib_log_info) clib_log_info("Doing X: %d", x);
    // ...
}
void clib_do_y(double y)
{
    if (clib_log_info) clib_log_info("Doing something else: %.3f", y);
    if (clib_log_error) clib_log_error("Uh-oh. An error has occurred: %s", "y is bad");
    // ...
}
///////////////////////////////////////////////////////////////////////////////

ac::jalog::Scope clibScope("clib");

void JalogForCLib_Info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ac::jalog::VPrintf(clibScope, ac::jalog::Level::Info, fmt, args);
    va_end(args);
}

void JalogForCLib_Error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ac::jalog::VPrintf(clibScope, ac::jalog::Level::Error, fmt, args);
    va_end(args);
}

int main()
{
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    clib_log_info = JalogForCLib_Info;
    clib_log_error = JalogForCLib_Error;

    AC_JALOG(Info, "Launching CLib");
    clib_do_x(43);
    clib_do_y(3.141592);

    return 0;
}
