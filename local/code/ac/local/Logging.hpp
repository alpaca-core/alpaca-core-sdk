// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// internal
// don't document
#if !defined(DOXYGEN)

#include <jalog/Scope.hpp>
#include <jalog/Log.hpp>

namespace ac::local::log {
extern jalog::Scope scope;
}

#define AC_LOCAL_LOG(lvl, ...) JALOG_SCOPE(::ac::local::log::scope, lvl, __VA_ARGS__)

#endif
