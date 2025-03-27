// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/SchemaVisitor.hpp>
#include <astl/tuple_util.hpp>

namespace ac::schema {

template <typename Dict, typename State>
Dict State_generateSchemaDict(State) {
    Dict dict;
    dict["id"] = State::id;
    dict["description"] = State::desc;

    auto& ops = dict["ops"];
    astl::tuple::for_each(typename State::Ops{}, [&]<typename Op>(Op) {
        auto& o = ops[Op::id];
        o["description"] = Op::desc;
        Struct_toSchema<typename Op::Params>(o["params"]);
        Struct_toSchema<typename Op::Return>(o["return"]);
    });

    auto& ins = dict["ins"];
    astl::tuple::for_each(typename State::Ins{}, [&]<typename Op>(Op) {
        auto& o = ins[Op::id];
        Struct_toSchema<typename Op::Type>(o["items"]);
        o["description"] = Op::desc;
    });

    auto& outs = dict["outs"];
    astl::tuple::for_each(typename State::Outs{}, [&]<typename Op>(Op) {
        auto& o = outs[Op::id];
        Struct_toSchema<typename Op::Type>(o["items"]);
        o["description"] = Op::desc;
    });

    return dict;
}

template <typename Dict, typename Interface>
Dict Interface_generateSchemaDict(Interface) {
    Dict dict;
    //dict["id"] = Interface::id;
    //dict["description"] = Interface::desc;

    //auto& states = dict["states"];
    //astl::tuple::for_each(typename Interface::States{}, [&]<typename State>(State) {
    //    states.push_back(State_generateSchemaDict<Dict>(State{}));
    //});

    return dict;
}

} // namespace ac::schema
