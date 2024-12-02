// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "DummyAInterface.hpp"
#include <tuple>

namespace ac::local::schema {

struct Dummy {
    static constexpr auto id = "dummy";
    struct Params {
        Field<std::string> spliceString = std::nullopt;

        template <typename Visitor>
        void visitFields(Visitor& v) {
            v(spliceString, "splice_string", "String to splice between model data elements");
        }
    };

    struct InstanceGeneral {
        static constexpr auto id = "general";

        struct Params {
            Field<int> cutoff = Default(0);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(cutoff, "cutoff", "Cutoff value");
            }
        };

        using Interfaces = std::tuple<DummyAInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::local::schema
