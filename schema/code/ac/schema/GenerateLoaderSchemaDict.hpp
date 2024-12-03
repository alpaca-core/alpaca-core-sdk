// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SchemaVisitor.hpp"
#include <astl/tuple_util.hpp>

namespace ac::local::schema {

template <typename Dict, typename Schema>
Dict generateLoaderSchema() {
    Dict dict;
    dict["id"] = Schema::id;
    dict["description"] = Schema::description;
    Struct_toSchema<typename Schema::Params>(dict["params"]);

    auto& is = dict["instances"];
    astl::tuple::for_each(typename Schema::Instances{}, [&]<typename Instance>(Instance) {
        auto& i = is[Instance::id];
        i["description"] = Instance::description;
        Struct_toSchema<typename Instance::Params>(i["params"]);
        auto& os = i["ops"];
        astl::tuple::for_each(typename Instance::Interfaces{}, [&]<typename Interface>(Interface) {
            astl::tuple::for_each(typename Interface::Ops{}, [&]<typename Op>(Op) {
                auto& o = os[Op::id];
                o["description"] = Op::description;
                Struct_toSchema<typename Op::Params>(o["params"]);
                Struct_toSchema<typename Op::Return>(o["return"]);
            });
        });
    });

    return dict;
}

}
