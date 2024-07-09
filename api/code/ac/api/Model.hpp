// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "JobPtr.hpp"
#include "Callback.hpp"
#include "Dict.hpp"

namespace ac::api {
class AC_API_EXPORT Model {
public:
    virtual ~Model();
    virtual void createJob(std::string_view jobType, Dict params, Callback<JobPtr> cb) = 0;
};
}
