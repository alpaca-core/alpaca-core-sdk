// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac::api {

class AC_API_EXPORT Provider {
public:
    virtual ~Provider();

    virtual void createModel(Dict params, Callback<ModelPtr> cb) = 0;
};

}
