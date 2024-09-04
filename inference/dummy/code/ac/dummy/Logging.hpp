// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <jalog/Scope.hpp>
#include <jalog/Log.hpp>

namespace ac::dummy::log {
extern jalog::Scope scope;
}

#define DUMMY_LOG(lvl, ...) JALOG_SCOPE(::ac::dummy::log::scope, lvl, __VA_ARGS__)
