// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "model.h"
#include "ModelPtr.hpp"

struct ac_model {
    ac::ModelPtr model;
};

namespace ac::cutil {

inline ac_model* ac_model_create(ModelPtr model) {
    return new ac_model{model};
}

} // namespace ac::cutil
