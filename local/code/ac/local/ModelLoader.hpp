// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelPtr.hpp"
#include "ModelDesc.hpp"
#include "ProgressCb.hpp"
#include <ac/Dict.hpp>

namespace ac::local {

class AC_LOCAL_EXPORT ModelLoader {
public:
    virtual ~ModelLoader();

    virtual ModelPtr loadModel(ModelDesc desc, Dict params, ProgressCb cb) = 0;
};

} // namespace ac::local
