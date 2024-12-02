// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "IOVisitors.hpp"
#include <ac/DictFwd.hpp>
#include <astl/tuple_util.hpp>

namespace ac::local::schema {

namespace impl {
struct FindById {
    std::string_view id;
    template <typename T>
    bool operator()(int, const T& elem) const {
        return elem.id == id;
    }
};
} // namespace impl

template <typename Ops, typename Dispatcher>
Dict dispatchOp(std::string_view opId, Dict&& opParams, Dispatcher& dispatcher) {
    Ops ops;
    return astl::tuple::find_if(ops, impl::FindById{opId},
        [&]<typename Op>(Op& op) {
            return Struct_toDict(dispatcher.on(op, Struct_fromDict<typename Op::Params>(std::move(opParams))));
        },
        [&] {
            return dispatcher.onNoOp(opId, opParams);
        }
    );
}

} // namespace ac::local::schema
