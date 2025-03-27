// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/OpTraits.hpp>
#include <ac/schema/Field.hpp>
#include <ac/schema/StateChange.hpp>

namespace ac::schema {

inline namespace test {

struct SomeOp {
    struct Params {
        Field<int> value;
    };

    struct Return {
        Field<int> result;
    };
};

struct StateTransitionOp {
    struct Params {
        Field<int> value;
    };
    using Return = StateChange;
};

static_assert(Op_isStateTransition<StateTransitionOp>);
static_assert(!Op_isStateTransition<SomeOp>);

} // namespace test

} // namespace ac::schema

