// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Dict.hpp>
#include <string_view>

namespace ac::local {
class Instance;

/// An inference model.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT Model {
public:
    virtual ~Model();

    /// Create an instance of the model.
    /// The returned instance is not bound to the model lifetime. It maintains everything needed inside.
    virtual std::unique_ptr<Instance> createInstance(std::string_view type, Dict params) = 0;
};
} // namespace ac::local
