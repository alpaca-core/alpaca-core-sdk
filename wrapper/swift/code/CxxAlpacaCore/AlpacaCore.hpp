// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"
#include "Dict.hpp"


namespace ac {
typedef void (*SwiftProgressCb)(void* _Nonnull context, float progress);
struct ProgressCallbackData {
    SwiftProgressCb _Nonnull m_cb;
    void* _Nonnull m_context;
};

static std::unique_ptr<local::ModelFactory> factorySingleton;

void initSDK();
class Model* _Nullable createModel(AlpacaCore::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData);

}
