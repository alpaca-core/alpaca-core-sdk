// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"
#include "Dict.hpp"

namespace ac {
typedef void (*SwiftProgressCb)(void* _Nonnull context, const char* _Nonnull tag, float progress);
struct ProgressCallbackData {
    SwiftProgressCb _Nonnull m_cb;
    void* _Nonnull m_context;
};

void initSDK();
class Model* _Nullable createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData);
std::vector<float> loadWavF32Mono(const std::string& path);

}
