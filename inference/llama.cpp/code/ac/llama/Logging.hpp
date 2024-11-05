// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace ac::llama::log {
extern jalog::Scope scope;
}

#define LLAMA_LOG(lvl, ...) AC_JALOG_SCOPE(::ac::llama::log::scope, lvl, __VA_ARGS__)
