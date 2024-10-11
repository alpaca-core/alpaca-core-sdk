// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"
#include "ACDict.hpp"


namespace ac {
typedef void (*SwiftProgressCb)(void* _Nonnull context, float progress);

static std::unique_ptr<local::ModelFactory> factorySingleton;

void initSDK();
class Model* _Nullable createModel(AlpacaCore::ModelDesc& desc, DictRef params, SwiftProgressCb _Nonnull cb, void* _Nonnull context);

}
