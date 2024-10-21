// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"
#include "Dict.hpp"

#include "ac-test-data-whisper-dir.h"


namespace ac {
typedef void (*SwiftProgressCb)(void* _Nonnull context, const char* _Nonnull tag, float progress);
struct ProgressCallbackData {
    SwiftProgressCb _Nonnull m_cb;
    void* _Nonnull m_context;
};

static std::unique_ptr<local::ModelFactory> factorySingleton;
static const char* _Nonnull WHISPER_DIR = AC_TEST_DATA_WHISPER_DIR;

void initSDK();
class Model* _Nullable createModel(AlpacaCoreSwift::ModelDesc& desc, DictRef params, ProgressCallbackData progressCbData);
std::vector<float> loadWavF32Mono(const std::string& path);

}
