// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <jalog/Scope.hpp>
#include <jalog/Log.hpp>

namespace ac::whisper::log {
extern jalog::Scope scope;
}

#define WHISPER_LOG(lvl, ...) JALOG_SCOPE(::ac::whisper::log::scope, lvl, __VA_ARGS__)
