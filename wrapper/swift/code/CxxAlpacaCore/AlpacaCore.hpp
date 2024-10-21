// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "Dict.hpp"
#include "Model.hpp"

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"

namespace ac::swift {

typedef void (*SwiftProgressCb)(void* _Nonnull context, const char* _Nonnull tag, float progress);

struct ProgressCallbackData {
    SwiftProgressCb _Nonnull m_cb;
    void* _Nonnull m_context;
};

void initSDK();

Model createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData);

std::vector<float> loadWavF32Mono(const std::string& path);

}
