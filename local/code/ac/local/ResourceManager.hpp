// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"
#include "ResourceLock.hpp"
#include <mutex>
#include <string>
#include <memory>
#include <vector>
#include <concepts>
#include <string_view>

namespace ac::local {

class AC_LOCAL_EXPORT ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <std::derived_from<Resource> R = Resource>
    ResourceLock<R> findResource(std::string_view key) const noexcept {
        return ResourceLock<R>(doFindResource(key));
    }

    template <std::derived_from<Resource> R>
    ResourceLock<R> addResource(std::string key, std::shared_ptr<R>&& resource) {
        doAddResource(std::move(key), resource);
        return ResourceLock<R>(std::move(resource));
    }

    // garbage collect expired resources
    // force: disregard expire time
    // return number of resources collected
    int garbageCollect(bool force = false);

    static void touchResourceExpiry(Resource& resource) noexcept;
private:
    struct ResourceData {
        std::string key;
        ResourcePtr resource;

        explicit operator bool() const noexcept { return !!resource; }
        void reset() {
            resource.reset();
        }
    };

    mutable std::mutex m_mutex;
    std::vector<ResourceData> m_resources; // sparse

    ResourcePtr doFindResource(std::string_view key) const noexcept;
    void doAddResource(std::string key, ResourcePtr resource);
};

} // namespace ac::local
