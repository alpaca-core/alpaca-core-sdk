// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Abort.hpp"
#include "SysFrameOpPrefix.h"
#include "FrameHelpers.hpp"
#include <ac/frameio/FrameWithStatus.hpp>
#include <cstddef>

namespace ac::schema {

inline namespace sys {

template <typename Io>
bool Abort_check(Io& io) {
    ac::frameio::FrameWithStatus res = io.get();
    if (!res.success()) {
        return false;
    }
    if (Frame_is(Abort{}, *res)) {
        return true;
    }
    throw std::runtime_error("Unexpected frame: " + res->op);
}

} // namespace sys

} // namespace ac::schema
