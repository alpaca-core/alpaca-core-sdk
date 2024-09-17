// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ModelDesc.hpp"
#include <ac/Dict.hpp>
#include <ac/Callbacks.hpp>
#include <string_view>
#include <functional>
#include <memory>

// inference interface for concrete inference instances
// the implementations of these interfaces are must be synchronous
// even though some take callbacks as arguments for compatibility, they need to be invoked in the same thread

namespace ac {

class AC_LOCAL_EXPORT LocalInferenceInstance {
public:
    virtual ~LocalInferenceInstance();

    virtual void runOpSync(std::string_view op, Dict params, BasicCb<Dict> streamCb, ProgressCb pcb) = 0;
};

class AC_LOCAL_EXPORT LocalInferenceModel {
public:
    virtual ~LocalInferenceModel();

    virtual std::unique_ptr<LocalInferenceInstance> createInstanceSync(std::string_view type, Dict params) = 0;
};

class AC_LOCAL_EXPORT LocalInferenceModelLoader {
public:
    virtual ~LocalInferenceModelLoader();

    virtual std::unique_ptr<LocalInferenceModel> loadModelSync(ModelDesc desc, Dict params, ProgressCb progress) = 0;
};

}