// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Printf.hpp"

// Macro which defines a function with signature:
// template <jalog::Level lvl>
// void(const char*, ...).
//
// It can be used for C libraries (or retro C++ ones) which have can be
// configured with a function pointer of this type.
//
// For example:
// struct CLibConfig {
//     void (*logInfo)(const char*, ...);
//     void (*logError)(const char*, ...);
//     // ...
// };
// AC_JALOG_DEFINE_PRINTF_FUNC(myprintf, myscope)
// ...
// CLibConfig cfg = {};
// cfg.logInfo = myprintf<jalog::Level::Info>;
// cfg.logError = myprintf<jalog::Level::Error>;
// ...
// CLibInit(&cfg);

#if defined(__GNUC__)
#   define I_AC_JALOG_PRINTF_WRAP_FMT __attribute__((format(printf, 1, 2)))
#else
#   define I_AC_JALOG_PRINTF_WRAP_FMT
#endif

#define AC_JALOG_DEFINE_PRINTF_FUNC(name, scope) \
    template <::ac::jalog::Level lvl> \
    I_AC_JALOG_PRINTF_WRAP_FMT void name(_Printf_format_string_ const char* format, ...) { \
        va_list args; \
        va_start(args, format); \
        ::ac::jalog::VPrintf(scope, lvl, format, args); \
        va_end(args); \
    }
