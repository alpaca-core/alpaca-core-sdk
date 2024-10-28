// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "Expected.hpp"

#include "AlpacaCore-Swift.h"

namespace AC {

typedef void (*SwiftProgressCb)(void* _Nonnull context, std::string tag, float progress);

struct ProgressCallbackData {
    SwiftProgressCb _Nullable m_cb = nullptr;
    void* _Nullable m_context = nullptr;
};

void initSDK();

class Model;
class DictRef;
Expected<Model, std::string> createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData);

}
