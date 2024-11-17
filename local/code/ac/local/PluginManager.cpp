// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "PluginManager.hpp"
#include "Logging.hpp"

namespace ac::local {

namespace {
PluginInterface openPlugin(const std::string& path);
void closePlugin(void* nativeHandle);
}

PluginManager::PluginManager(ModelLoaderRegistry& registry)
    : m_registry(registry)
{}

PluginManager::~PluginManager() = default;

} // namespace ac::local
