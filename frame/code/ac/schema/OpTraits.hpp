// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "StateChange.hpp"
#include <concepts>

namespace ac::schema {

template <typename Op>
concept Op_isStateTransition = std::same_as<typename Op::Return, sys::StateChange>;

template <typename Op>
struct OpParams {
    static constexpr auto id = Op::id;
    using Type = typename Op::Params;
};

template <typename Op>
struct SimpleOpReturn {
    static constexpr auto id = Op::id;
    using Type = typename Op::Return;
};

template <typename Op>
using OpReturn = std::conditional_t<Op_isStateTransition<Op>, sys::StateChange, SimpleOpReturn<Op>>;

} // namespace ac::schema
