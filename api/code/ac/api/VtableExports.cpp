// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Provider.hpp"
#include "Model.hpp"
#include "Job.hpp"

// export vtables for classes which only have that

namespace ac::api {
Provider::~Provider() = default;
Model::~Model() = default;
Job::~Job() = default;
}