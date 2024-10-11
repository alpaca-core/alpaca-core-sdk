// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "IntrusiveRefCounted.hpp"
#include "ACDict.hpp"
#include "AlpacaCore.hpp"

#include <ac/local/Instance.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

namespace ac {

Instance::Instance(std::unique_ptr<local::Instance> instance)
    : m_instance(std::move(instance))
{}

DictRoot* Instance::runOp(const std::string& op, DictRef params, SwiftProgressCb _Nonnull cb, void* _Nonnull context) {
    DictRoot* root = DictRoot::create();
    DictRef ref = root->getRef();
    ref.getDict() = m_instance->runOp(op, params.getDict(), [&](std::string_view tag, float progress) {
        cb(context, progress);
        return true;
    });
    return root;
}

}

void retainModel(ac::Model* _Nullable d);
void releaseModel(ac::Model* _Nullable d);
