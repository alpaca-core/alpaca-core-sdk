// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "IOVisitors.hpp"
#include <ac/DictFwd.hpp>
#include <astl/tuple_util.hpp>
#include <astl/qalgorithm.hpp>

namespace ac::local::schema {

struct OpDispatcherData {
    struct HandlerData {
        std::string_view id;
        std::function<Dict(Dict&&)> handler;
    };
    std::vector<HandlerData> handlers;

    template <typename Op, typename Handler>
    void registerHandler(Op, Handler& h) {
        auto stronglyTypedCall = [&](Dict&& params) {
            return Struct_toDict(h.on(Op{}, Struct_fromDict<typename Op::Params>(std::move(params))));
        };
        handlers.push_back({Op::id, std::move(stronglyTypedCall)});
    }

    std::optional<Dict> dispatch(std::string_view id, Dict&& params) {
        auto h = astl::pfind_if(handlers, [&](const HandlerData& item) {
            return item.id == id;
        });
        if (!h) return {};
        return h->handler(std::move(params));
    }
};

template <typename Ops, typename Handler>
void registerHandlers(OpDispatcherData& data, Handler& h) {
    Ops ops{};
    astl::tuple::for_each(ops, [&]<typename Op>(Op op) {
        data.registerHandler(op, h);
    });
}

} // namespace ac::local::schema
