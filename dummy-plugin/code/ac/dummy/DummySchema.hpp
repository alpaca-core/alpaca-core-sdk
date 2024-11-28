// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "DummyAInterface.hpp"
#include <tuple>

namespace ac::local::schema {

struct DummySchema {
    static constexpr auto id = "dummy";

    struct InstanceGeneral {
        static constexpr auto id = "general";

        struct Params {
            int cutoff = 0;

            template <typename Visitor>
            void visit(Visitor& v) {
                v(cutoff, "cutoff", "Cutoff value", true);
            }
        };

        using Interfaces = std::tuple<DummyAInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::local::schema
