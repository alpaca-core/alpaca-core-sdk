// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"
#include <ac/xec/strand.hpp>
#include <ac/xec/timer_ptr.hpp>
#include <ac/xec/completion_cb.hpp>
#include <mutex>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <concepts>
#include <string_view>

namespace ac::local {

using ResourcePtr = std::shared_ptr<Resource>;

class AC_LOCAL_EXPORT ResourceManager {
public:
    ResourceManager(xec::strand ex);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <std::derived_from<Resource> R>
    class ResourceLock {
    public:
        explicit operator bool() const noexcept { return !!m_resource; }

        R* get() const noexcept { return static_cast<R*>(m_resource.get()); }
        R* operator->() const noexcept { return get(); }
        R& operator*() const noexcept { return *get(); }

        ~ResourceLock() {
            if (m_resource) {
                m_manager.unlockResource(std::move(m_resource));
            }
        }
    private:
        friend class ResourceManager;
        ResourceLock(ResourceManager& manager, const std::shared_ptr<R>& resource)
            : m_manager(manager)
            , m_resource(resource)
        {}
        ResourceManager& m_manager;
        std::shared_ptr<R> m_resource;
    };

    template <std::derived_from<Resource> R>
    ResourceLock<R> lock(std::string_view key) {
        auto resource = lockResource(key);
        return {*this, std::static_pointer_cast<R>(resource)};
    }

    void requestFreeSpace(std::string_view space, xec::completion_cb cb);

    void addResource(ResourcePtr&& resource, std::string key);

private:
    ResourcePtr lockResource(std::string_view key);
    void unlockResource(ResourcePtr ptr);

    struct ResourceData {
        std::string key;
        std::chrono::steady_clock::time_point expireTime;
        ResourcePtr resource;
        xec::strand freeExecutor;
    };

    std::mutex m_mutex;
    xec::timer_ptr m_timer;
    std::vector<ResourceData> m_resources; // sparse
};

} // namespace ac::local
