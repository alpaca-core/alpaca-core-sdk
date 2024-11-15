// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Lib.hpp"
#include "ModelLoaderRegistry.hpp"

namespace ac::local {

namespace {
ModelLoaderRegistry g_modelLoaderRegistry;
}

ModelLoaderRegistry& Lib::modelLoaderRegistry() {
    return g_modelLoaderRegistry;
}

} // namespace ac::local
