// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <jalog/Scope.hpp>
#include <jalog/Log.hpp>

namespace ac::llama::log {
extern jalog::Scope scope;
}

#define LLAMA_LOG(lvl, ...) JALOG_SCOPE(::ac::llama::log::scope, lvl, __VA_ARGS__)
