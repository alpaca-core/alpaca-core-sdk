// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#if !defined(AC_JALOG_ENABLED)
#   define AC_JALOG_ENABLED 1
#endif

#if AC_JALOG_ENABLED

#include "DefaultScope.hpp"
#include "Stream.hpp"

#define AC_JALOG_STREAM_SCOPE(scope, lvl) ::ac::jalog::Stream(scope, ::ac::jalog::Level::lvl)
#define AC_JALOG_STREAM(lvl) AC_JALOG_STREAM_SCOPE(::ac::jalog::Default_Scope, lvl)

#else

#include "NoopStream.hpp"

#define AC_JALOG_STREAM_SCOPE(...) ::ac::jalog::NoopStream()
#define AC_JALOG_STREAM(...) ::ac::jalog::NoopStream()

#endif

