// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ResourcePtr.hpp"
#include <astl/ufunction.hpp>
#include <astl/expected.hpp>
#include <ac/xec/strand.hpp>
#include <memory>

namespace ac::local {
struct ResourceQuery {
    struct CreationError {
        enum Type {
            NoSpace,
            Other
        };
        Type type;
        std::string message;

        static CreationError makeNoSpaceError() { return CreationError{NoSpace, "No space"}; }
        static CreationError mahtOtherError(std::string msg) { return CreationError{Other, std::move(msg)}; }

        bool noSpace() const { return type == NoSpace; }
    };
    using Result = astl::expected<ResourcePtr, CreationError>;

    std::string key;

    astl::ufunction<Result()> tryCreateFunc;
    astl::ufunction<void(Result)> queryCompleteFunc;
};

using ResourceQueryPtr = std::unique_ptr<ResourceQuery>;

} // namespace ac::local
