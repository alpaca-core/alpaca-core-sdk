// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "StreamPtr.hpp"
#include "IoPtr.hpp"
#include <ac/xec/post.hpp>
#include <memory>

namespace ac::frameio {

class IoExecutor {
public:
    ac::xec::strand strand;

    IoExecutor() = default;
    explicit IoExecutor(ac::xec::strand s) : strand(std::move(s)) {}

    void post(xec::task task) const {
        ac::xec::post(strand, std::move(task));
    }

    InputPtr attachInput(ReadStreamPtr stream) const {
        return std::make_unique<Input>(std::move(stream), strand);
    }
    OutputPtr attachOutput(WriteStreamPtr stream) const {
        return std::make_unique<Output>(std::move(stream), strand);
    }
};

} // namespace ac::frameio
