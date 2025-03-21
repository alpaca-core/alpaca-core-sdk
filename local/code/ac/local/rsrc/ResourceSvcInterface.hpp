// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>

namespace ac::schema {

inline namespace rsrc {

struct ResourceSvcState {
    static constexpr auto id = "resource";
    static constexpr auto desc = "Resource service state";

    struct OpRequestResourceGc {
        static constexpr auto id = "request-gc";
        static constexpr auto desc = "Request resource garbage collection";

        struct Params {
            Field<std::string> type = Default();
            Field<bool> force = Default(false);

            void visitFields(impl::DummyVisitor&) {
                Default(type, "type", "Resource type to garbage collect");
                Default(force, "force", "Force garbage collection disregarding expire time");
            }
        };

        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpRequestResourceGc>;
    using Callbacks = std::tuple<OpRequestResourceGc>;
};

} // namespace rsrc

} // namespace ac::schema