// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>
#include <string_view>

namespace ac::local {

/// An inference instance.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT Instance {
public:
    virtual ~Instance();

    /// Run an operation on the instance.
    /// The optional progress callback can be used to report the progress of the operation, but note that most
    /// operations completely ignore that arg. Whether one makes use of it should be documented in the operation's API.
    ///
    /// The returned dictionary contains the result of the operation as per the operation's API.
    virtual Dict runOp(std::string_view op, Dict params, ProgressCb cb = {}) = 0;
};

} // namespace ac::local
