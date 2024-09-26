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

class ModelFactory {
public:
    ModelPtr createModel(ModelDesc desc, Dict params, ProgressCb cb = {});
};

} // namespace ac::local
