// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <concepts>

namespace ac::schema {

inline namespace sys { struct StateChange; }

template <typename Op>
concept Op_isStateTransition = std::same_as<typename Op::Return, sys::StateChange>;

} // namespace ac::schema
