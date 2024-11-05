// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// internal
// don't document
#if !defined(DOXYGEN)

#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace ac::local::log {
extern jalog::Scope scope;
}

#define AC_LOCAL_LOG(lvl, ...) AC_JALOG_SCOPE(::ac::local::log::scope, lvl, __VA_ARGS__)

#endif
