// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#if !defined(AC_JALOG_ENABLED)
#   define AC_JALOG_ENABLED 1
#endif

#if AC_JALOG_ENABLED

#include "DefaultScope.hpp"
#include "Printf.hpp"

#define AC_JALOG_PRINTF_SCOPE(scope, lvl, fmt, ...) \
    if (scope.enabled(::ac::jalog::Level::lvl)) \
        ::ac::jalog::PrintfUnchecked(scope, ::ac::jalog::Level::lvl, fmt, ##__VA_ARGS__)

#define AC_JALOG_PRINTF(lvl, ...) AC_JALOG_PRINTF_SCOPE(::ac::jalog::Default_Scope, lvl, __VA_ARGS__)

#else

#define AC_JALOG_PRINTF_SCOPE(...)
#define AC_JALOG_PRINTF(...)

#endif

