// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ResourceLock.hpp"

#include <ac/xec/post.hpp>
#include <astl/qalgorithm.hpp>

#include <string>
#include <memory>
#include <vector>
#include <concepts>
#include <string_view>
#include <cassert>

namespace ac::local {

template <typename Key, typename R>
class ResourceManager {
    static_assert(std::derived_from<R, Resource>);
public:
    ResourceManager() = default;
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    ResourceLock<R> find(const Key& key) const noexcept {
        auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return r && r.key == key; });
        if (!p) return {};
        return ResourceLock<R>(p->resource);
    }

    ResourceLock<R> add(Key key, std::shared_ptr<R>&& resource) {
        assert(!find(key));

        auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return !r; });

        if (!p) {
            // no free slot was found
            p = &m_resources.emplace_back();
        }

        p->key = std::move(key);
        p->resource = std::move(resource);

        return ResourceLock<R>(p->resource);
    }

    template <typename Factory>
    ResourceLock<R> findOrCreate(Key key, Factory factory) {
        if (auto f = find(key)) {
            return f;
        }
        std::shared_ptr<R> newResource = factory(key);
        return add(std::move(key), std::move(newResource));
    }

    // garbage collect expired resources
    // force: disregard expire time
    // return number of resources collected
    int garbageCollect(bool force = false) {
        // if force is true, we set the now to the end of time and now everyone is expired
        auto now = force ? std::chrono::steady_clock::time_point::max() : std::chrono::steady_clock::now();

        int count = 0;
        for (auto& rd : m_resources) {
            if (rd.resource.use_count() != 1) {
                // either empty or shared
                continue;
            }
            if (rd.resource->expireTime() <= now) {
                rd.reset();
                ++count;
            }
        }

        return count;
    }

private:
    struct ResourceData {
        Key key;
        std::shared_ptr<R> resource;

        explicit operator bool() const noexcept { return !!resource; }
        void reset() {
            // intantionally don't touch key at this point as it's useful for debugging
            resource.reset();
        }
    };

    std::vector<ResourceData> m_resources; // sparse
};

} // namespace ac::local
