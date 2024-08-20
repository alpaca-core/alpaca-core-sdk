// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac {

class AC_API_EXPORT Provider {
public:
    virtual ~Provider();

    virtual void createModel(std::string_view id, Dict params, Callback<ModelPtr> cb) = 0;
};

}
