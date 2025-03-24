// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ResourceLock.hpp"
#include "ResourceManager.hpp"

#include <astl/qalgorithm.hpp>

#include <string>
#include <memory>
#include <vector>
#include <concepts>
#include <string_view>
#include <cassert>

namespace ac::local {

template <typename Key, typename R>
class ResourceCache {
    static_assert(std::derived_from<R, Resource>);
public:
    ResourceCache(ResourceManager& manager)
        : m_manager(manager)
    {}
    ~ResourceCache() = default;

    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;

    ResourceLock<R> find(const Key& key) const noexcept {
        auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return r.key == key; });
        if (!p) return {};
        return ResourceLock<R>(p->resource.lock());
    }

    ResourceLock<R> add(Key key, std::shared_ptr<R>&& resource) {
        assert(!find(key));

        auto p = astl::pfind_if(m_resources, [&](const ResourceData& r) { return r.resource.expired(); });

        if (!p) {
            // no free slot was found
            p = &m_resources.emplace_back();
        }

        p->key = std::move(key);
        p->resource = resource;
        m_manager.add(resource);

        return ResourceLock<R>(std::move(resource));
    }

    template <typename Factory>
    ResourceLock<R> findOrCreate(Key key, Factory factory) {
        if (auto f = find(key)) {
            return f;
        }
        std::shared_ptr<R> newResource = factory(key);
        return add(std::move(key), std::move(newResource));
    }

    ResourceManager& manager() noexcept { return m_manager; }

private:
    struct ResourceData {
        Key key;
        std::weak_ptr<R> resource;

        explicit operator bool() const noexcept { return !!resource; }
    };

    ResourceManager& m_manager;
    std::vector<ResourceData> m_resources; // sparse
};

} // namespace ac::local
