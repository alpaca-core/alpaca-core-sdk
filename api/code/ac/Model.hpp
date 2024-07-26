// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "InstancePtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac {
class AC_API_EXPORT Model {
public:
    virtual ~Model();
    virtual void createInstance(std::string_view type, Dict params, Callback<InstancePtr> cb) = 0;
};
}
