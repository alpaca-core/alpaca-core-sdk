// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/SchemaVisitor.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <utility>

namespace ac::local {

template <typename Instance>
auto Model_createInstance(Model& model, typename Instance::Params p) {
    return model.createInstance(Instance::id, schema::Struct_toDict(std::move(p)));
}

template <typename Op>
typename Op::Return Instance_runOp(Instance& instance, typename Op::Params p, ProgressCb cb = {}) {
    return schema::Struct_fromDict<typename Op::Return>(
        instance.runOp(Op::id, schema::Struct_toDict(std::move(p)), std::move(cb))
    );
}

} // namespace ac::local
