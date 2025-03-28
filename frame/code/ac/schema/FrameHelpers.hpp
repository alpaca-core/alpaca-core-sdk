// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SchemaVisitor.hpp"
#include "../Frame.hpp"
#include <cassert>

namespace ac::schema {

template <typename T>
bool Frame_is(T, const Frame& frame) {
    return frame.op == T::id;
}

template <typename T>
Frame Frame_from(T, typename T::Type t) {
    Frame ret;
    ret.op = T::id;
    ret.data = Struct_toDict(std::move(t));
    return ret;
}

template <typename T>
typename T::Type Frame_to(T, Frame f) {
    if (!Frame_is(T{}, f)) {
        throw_ex{} << "expected frame " << T::id << ", but got " << f.op;
    }
    return Struct_fromDict<typename T::Type>(std::move(f.data));
}

template <typename T>
std::optional<typename T::Type> Frame_optTo(T, Frame& f) {
    if (!Frame_is(T{}, f)) {
        return std::nullopt;
    }
    return Struct_fromDict<typename T::Type>(std::move(f.data));
}

} // namespace ac::schema
