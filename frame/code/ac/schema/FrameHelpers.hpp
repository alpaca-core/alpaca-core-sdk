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
    return schema::Struct_fromDict<typename Op::Return>(std::move(f.data));
}

template <typename Stream>
Frame Frame_fromStreamType(Stream, typename Stream::Type s) {
    Frame ret;
    ret.op = Stream::id;
    ret.data = schema::Struct_toDict(std::move(s));
    return ret;
}

template <typename Stream>
typename Stream::Type Frame_toStreamType(Stream, Frame f) {
    return schema::Struct_fromDict<typename Stream::Type>(std::move(f.data));
}

} // namespace ac::schema
