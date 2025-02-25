// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Frame.hpp"

namespace ac {

inline Frame Frame_stateChange(std::string_view stateName) {
    Frame ret;
    ret.op.reserve(2 + stateName.size());
    ret.op += "s:";
    ret.op += stateName;
    return ret;
}

inline bool Frame_isStateChange(const Frame& f) {
    return f.op.size() >= 2 && f.op[0] == 's' && f.op[1] == ':';
}

inline std::string_view Frame_getStateChange(const Frame& f) {
    if (Frame_isStateChange(f)) {
        return {f.op.data() + 2, f.op.size() - 2};
    }
    return {};
}

template<typename StreamOp>
inline bool Frame_getStreamData(const Frame& f, typename StreamOp::Type& out) {
    if (f.op != StreamOp::id) {
        return false;
    }

    out = f.data.get<typename StreamOp::Type>();
    return true;
}

inline Frame Frame_error(std::string error) {
    return {"error", std::move(error)};
}

inline bool Frame_isError(const Frame& f) {
    return f.op == "error";
}

inline std::string_view Frame_getError(const Frame& f) {
    if (Frame_isError(f)) {
        return f.data.get<std::string_view>();
    }
    return {};
}

} // namespace ac
