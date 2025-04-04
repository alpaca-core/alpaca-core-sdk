// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "coro_iterator.hpp"

#define co_for(iter, generator) \
    for (auto iter = co_await make_coro_iterator(generator); !iter.done(); co_await iter.next())
