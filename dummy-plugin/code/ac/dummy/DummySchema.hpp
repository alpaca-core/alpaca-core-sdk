// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "DummyAInterface.hpp"
#include <tuple>

namespace ac::local::schema {

struct Dummy {
    static constexpr auto id = "dummy";
    using Params = std::nullptr_t;

    struct InstanceGeneral {
        static constexpr auto id = "general";

        struct Params {
            Field<int> cutoff = Default(0);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(cutoff, "cutoff", "Cutoff value", true);
            }
        };

        using Interfaces = std::tuple<DummyAInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::local::schema
