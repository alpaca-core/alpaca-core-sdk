// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "DummyInterface.hpp"
#include <tuple>

namespace ac::local::schema {

struct DummyLoader {
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
            Field<int> cutoff = Default(-1);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(cutoff, "cutoff", "Cut off model data to n-th element (or don't cut if -1)");
            }
        };

        using Interfaces = std::tuple<DummyInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::local::schema
