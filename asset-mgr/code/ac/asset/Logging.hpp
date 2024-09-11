// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <jalog/Scope.hpp>
#include <jalog/Log.hpp>

namespace ac::asset::log {
extern jalog::Scope scope;
}

#define AC_ASSET_LOG(lvl, ...) JALOG_SCOPE(::ac::asset::log::scope, lvl, __VA_ARGS__)
