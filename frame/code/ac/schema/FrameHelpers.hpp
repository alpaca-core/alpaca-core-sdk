// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SchemaVisitor.hpp"
#include "../Frame.hpp"

namespace ac::schema {

template <typename Op>
Frame Frame_fromOpParams(Op, typename Op::Params p) {
    Frame ret;
    ret.op = Op::id;
    ret.data = schema::Struct_toDict(std::move(p));
    return ret;
}

template <typename Op>
typename Op::Return Frame_toOpReturn(Op, Frame f) {
    assert(f.op == Op::id);
    return schema::Struct_fromDict<typename Op::Return>(std::move(f.data));
}

} // namespace ac::schema
