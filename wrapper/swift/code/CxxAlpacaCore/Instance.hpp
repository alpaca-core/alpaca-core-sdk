// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/Instance.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

#include "AlpacaCore.hpp"
#include "IntrusiveRefCounted.hpp"
#include "ACDict.hpp"

namespace ac {

class Instance : public IntrusiveRefCounted<Model> {
public:
    DictRoot* _Nonnull runOp(const std::string& op, DictRef params, ProgressCb _Nullable cb);
private:
    friend class Model;
    Instance(std::unique_ptr<local::Instance> instance);
    std::unique_ptr<local::Instance> m_instance;
} SWIFT_SHARED_REFERENCE(retainModel, releaseModel);
}

void retainModel(ac::Model* _Nullable d);
void releaseModel(ac::Model* _Nullable d);
