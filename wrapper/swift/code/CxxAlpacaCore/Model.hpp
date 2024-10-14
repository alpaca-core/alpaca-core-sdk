// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/Model.hpp>
#include <ac/local/ModelPtr.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

#include "IntrusiveRefCounted.hpp"
#include "Dict.hpp"

namespace ac {
class Instance;

class Model : public IntrusiveRefCounted<Model> {
public:
    Model(local::ModelPtr model);
    static Model* _Nonnull create(local::ModelPtr model);

    Instance* _Nonnull createInstance(const std::string& type, DictRef params);
private:
    local::ModelPtr m_model;
} SWIFT_SHARED_REFERENCE(retainModel, releaseModel);
}

void retainModel(ac::Model* _Nullable d);
void releaseModel(ac::Model* _Nullable d);
