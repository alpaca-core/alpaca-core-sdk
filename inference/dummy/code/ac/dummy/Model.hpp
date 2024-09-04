// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <string>
#include <functional>

namespace ac::dummy {
using ModelLoadProgressCb = std::function<void(float)>;

class AC_DUMMY_EXPORT Model {
public:
    struct Params {

    };

    Model(std::string path, ModelLoadProgressCb loadProgressCb, Params params);
    ~Model();
};

} // namespace ac::dummy
