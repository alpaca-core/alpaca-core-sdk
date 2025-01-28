// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Frame.hpp"

namespace ac {
Frame Frame_stateChange(std::string_view stateName) {
    Frame ret;
    ret.op.reserve(2 + stateName.size());
    ret.op += "s:";
    ret.op += stateName;
    return ret;
}

bool Frame_isStateChange(const Frame& f) {
    return f.op.size() >= 2 && f.op[0] == 's' && f.op[1] == ':';
}

std::string_view Frame_getStateChange(const Frame& f) {
    if (Frame_isStateChange(f)) {
        return {f.op.data() + 2, f.op.size() - 2};
    }
    return {};
}

} // namespace ac
