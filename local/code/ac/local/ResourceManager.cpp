// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ResourceManager.hpp"
#include "Logging.hpp"
#include <ac/xec/timer.hpp>

namespace ac::local {

ResourceManager::ResourceManager(xec::strand ex)
    : m_timer(xec::timer::create(std::move(ex)))
{}

ResourceManager::~ResourceManager() = default;

void ResourceManager::requestFreeSpace(std::string_view space, xec::completion_cb cb) {

}

void ResourceManager::addResource(ResourcePtr&& resource, std::string key) {

}

ResourcePtr ResourceManager::lockResource(std::string_view key) {
    std::lock_guard lock(m_mutex);
    for (auto& r : m_resources) {
        if (r.key == key) {
            return r.resource;
        }
    }
}

void ResourceManager::unlockResource(ResourcePtr ptr) {
    std::lock_guard lock(m_mutex);
}

} // namespace ac::local
