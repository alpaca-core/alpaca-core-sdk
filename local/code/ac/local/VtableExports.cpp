// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelLoader.hpp"
#include "Model.hpp"
#include "Instance.hpp"

// export vtables for classes which only have that

namespace ac::local {
ModelLoader::~ModelLoader() = default;
Model::~Model() = default;
Instance::~Instance() = default;
} // namespace ac::local
