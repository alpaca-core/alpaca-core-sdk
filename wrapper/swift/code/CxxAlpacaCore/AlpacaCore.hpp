// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>

#include "AlpacaCore-Swift.h"
#include "ACDict.hpp"

typedef void (*ProgressCb)(float progress);

namespace ac {
static std::unique_ptr<local::ModelFactory> factorySingleton;

void initSDK();
class Model* createModel(AlpacaCore::ModelDesc& desc, DictRef params, ProgressCb pcb);

}
