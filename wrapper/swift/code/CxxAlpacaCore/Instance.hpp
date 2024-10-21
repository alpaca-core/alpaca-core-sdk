// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/Instance.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

#include "AlpacaCore.hpp"
#include "IntrusiveRefCounted.hpp"
#include "Dict.hpp"

namespace ac::swift {

class Instance : public IntrusiveRefCounted<Instance> {
public:
    DictRoot* _Nonnull runOp(const std::string& op, DictRef params, ProgressCallbackData progressCbData);
private:
    friend class Model;
    Instance(std::unique_ptr<local::Instance> instance);
    std::unique_ptr<local::Instance> m_instance;
} SWIFT_SHARED_REFERENCE(retainInstance, releaseInstance);
}

void retainInstance(ac::swift::Instance* _Nullable d);
void releaseInstance(ac::swift::Instance* _Nullable d);
