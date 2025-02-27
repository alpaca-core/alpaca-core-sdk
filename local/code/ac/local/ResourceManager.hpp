// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"
#include "ResourceLock.hpp"

#include <ac/xec/post.hpp>
#include <astl/qalgorithm.hpp>

#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <concepts>
#include <string_view>

namespace ac::local {

template<typename ResourceKey>
class AC_LOCAL_EXPORT ResourceManager {
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <std::derived_from<Resource> R = Resource>
    ResourceLock<R> findResource(ResourceKey key) const noexcept {
        return ResourceLock<R>(doFindResource(key));
    }

    template <std::derived_from<Resource> R>
    ResourceLock<R> addResource(ResourceKey key, std::shared_ptr<R>&& resource) {
        doAddResource(std::move(key), resource);
        return ResourceLock<R>(std::move(resource));
    }

    template <std::derived_from<Resource> R, typename CreationCallback>
    ResourceLock<R> findOrCreateResource(ResourceKey key, CreationCallback creationCallback) {
        {
            std::lock_guard l(m_mutex);
            auto result = doFindResourceL(key);
            if (result) {
                return ResourceLock<R>(std::move(result));
            }
        }

        ResourcePtr ptr = creationCallback();

        {
            std::lock_guard l(m_mutex);
            auto check = doFindResourceL(key);
            if (!check) {
                doAddResourceL(key, ptr);
                return ResourceLock<R>(std::move(ptr));
            }
        }
    }

    // garbage collect expired resources
    // force: disregard expire time
    // return number of resources collected
    int garbageCollect(bool force = false) {
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

private:
    struct ResourceData {
        ResourceKey key;
        ResourcePtr resource;

        explicit operator bool() const noexcept { return !!resource; }
        void reset() {
            resource.reset();
        }
    };

    mutable std::mutex m_mutex;
    std::vector<ResourceData> m_resources; // sparse

    ResourcePtr doFindResource(ResourceKey key) const noexcept {
        std::lock_guard l(m_mutex);
        return doFindResourceL(key);
    }

    ResourcePtr doFindResourceL(ResourceKey key) const noexcept {
        auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return r && r.key == key; });
        if (!p) return {};
        return p->resource;
    }

    void doAddResource(ResourceKey key, ResourcePtr resource) {
        std::lock_guard l(m_mutex);
        doAddResourceL(key, resource);
    }

    void doAddResourceL(ResourceKey key, ResourcePtr resource) {
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

};

} // namespace ac::local
