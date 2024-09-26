// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>
#include <string_view>
#include <optional>

namespace ac::local {
class Instance {
public:
    virtual ~Instance();

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb cb = {}) = 0;

    virtual bool haveStream() const noexcept = 0;
    virtual void pushStream(Dict input) = 0;
    virtual std::optional<Dict> pullStream() = 0;
};
} // namespace ac::local
