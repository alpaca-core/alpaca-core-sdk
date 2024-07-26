// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/pp_select.h>

#define I_ASTL_MOVE_CAPTURE_ONE(a, i) a = std::move(a)

#define movecap(...) SPLAT_ITERATE_WITH(I_ASTL_MOVE_CAPTURE_ONE, ##__VA_ARGS__)
