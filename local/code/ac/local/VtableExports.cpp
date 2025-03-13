// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Service.hpp"
#include "ServiceFactory.hpp"
#include "ServiceScorer.hpp"

// export vtables for classes which only have that

namespace ac::local {
Service::~Service() = default;
ServiceFactory::~ServiceFactory() = default;
ServiceScorer::~ServiceScorer() = default;
} // namespace ac::local
