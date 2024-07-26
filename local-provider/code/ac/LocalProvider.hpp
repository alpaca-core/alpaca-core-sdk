// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Provider.hpp>

namespace ac {

class AC_LOCAL_EXPORT LocalProvider : public Provider {
public:
    virtual void createModel(Dict params, Callback<ModelPtr> cb) override;
};

}