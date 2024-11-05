// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace ac::dummy::log {
extern jalog::Scope scope;
}

#define DUMMY_LOG(lvl, ...) AC_JALOG_SCOPE(::ac::dummy::log::scope, lvl, __VA_ARGS__)
