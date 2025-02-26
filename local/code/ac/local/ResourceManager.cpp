// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ResourceManager.hpp"
#include "Logging.hpp"
#include <ac/xec/post.hpp>
#include <astl/qalgorithm.hpp>

namespace ac::local {

ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

ResourcePtr ResourceManager::doFindResource(std::string_view key) const noexcept {
    std::lock_guard l(m_mutex);
    auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return r && r.key == key; });
    if (!p) return {};
    return p->resource;
}

void ResourceManager::doAddResource(std::string key, ResourcePtr resource) {
    std::lock_guard l(m_mutex);

    // we intentionally don't check for key duplicates
    // if someone adds a duplicate key, it is on their head

    auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return !r; });

    if (!p) {
        // no free slot was found
        p = &m_resources.emplace_back();
    }

    p->key = std::move(key);
    p->resource = std::move(resource);
}

int ResourceManager::garbageCollect(bool force) {
    std::lock_guard l(m_mutex);

    // if force is true, we set the now to the end of time and now everyone is expired
    auto now = force ? std::chrono::steady_clock::time_point::max() : std::chrono::steady_clock::now();

    int count = 0;
    for (auto& rd : m_resources) {
        if (rd.resource.use_count() != 1) {
            // either empty or shared
            continue;
        }
        if (rd.resource->expireTime <= now) {
            rd.reset();
            ++count;
        }
    }

    return count;
}

void ResourceManager::touchResourceExpiry(Resource& resource) noexcept {
    resource.expireTime = std::chrono::steady_clock::now() + resource.maxAge;
}

namespace impl {
void Resource_touch(Resource& resource) {
    ResourceManager::touchResourceExpiry(resource);
}
} // namespace impl

} // namespace ac::local
