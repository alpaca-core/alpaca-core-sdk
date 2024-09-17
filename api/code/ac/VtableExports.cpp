// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include "Instance.hpp"

// export vtables for classes which only have that

namespace ac {
Model::~Model() = default;
Instance::~Instance() = default;
}