// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ProviderScorer.hpp"
#include "Provider.hpp"

// export vtables for classes which only have that

namespace ac::local {
ProviderScorer::~ProviderScorer() = default;
Provider::~Provider() = default;
} // namespace ac::local
