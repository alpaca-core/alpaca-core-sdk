// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <utility>

namespace ac::local {

template <typename Instance>
auto Model_createInstance(Model& model, typename Instance::Params p) {
    return model.createInstance(Instance::id, p.toDict());
}

template <typename Op>
typename Op::Return Instance_runOp(Instance& instance, typename Op::Params p, ProgressCb cb = {}) {
    return Op::Return::fromDict(instance.runOp(Op::id, p.toDict(), std::move(cb)));
}

} // namespace ac::local
