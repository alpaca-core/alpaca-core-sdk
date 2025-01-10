// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ProviderScorer.hpp"
#include "Provider.hpp"
#include "Model.hpp"
#include "Instance.hpp"

// export vtables for classes which only have that

namespace ac::local {
ProviderScorer::~ProviderScorer() = default;
Provider::~Provider() = default;
Model::~Model() = default;
Instance::~Instance() = default;
} // namespace ac::local
