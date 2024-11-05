// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#if !defined(AC_JALOG_ENABLED)
#   define AC_JALOG_ENABLED 1
#endif

#if AC_JALOG_ENABLED

#include "DefaultScope.hpp"
#include "BasicStream.hpp"

#define AC_JALOG_SCOPE(scope, lvl, ...) \
    if (scope.enabled(::ac::jalog::Level::lvl)) \
        ::ac::jalog::BasicStream(scope, ::ac::jalog::Level::lvl), __VA_ARGS__, ::ac::jalog::endl

#define AC_JALOG(lvl, ...) AC_JALOG_SCOPE(::ac::jalog::Default_Scope, lvl, __VA_ARGS__)

#else

#define AC_JALOG_SCOPE(...)
#define AC_JALOG(...)

#endif

