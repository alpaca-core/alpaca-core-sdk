// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/xec/strand.hpp>
#include <ac/xec/timer_ptr.hpp>
#include <string>
#include <memory>
#include <chrono>
#include <deque>
#include <concepts>

namespace ac::local {

class AC_LOCAL_EXPORT ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    template <typename R>
    class ResourceLock {
    public:
        ~ResourceLock() {

        }

        R* get() const noexcept { return static_cast<R*>(m_resource.get()); }
        R* operator->() const noexcept { return get(); }
        R& operator*() const noexcept { return *get(); }

    private:
        ResourceData m_data;
    };


private:
    using ResourcePtr = std::shared_ptr<void>;

    ResourcePtr lockResource(const std::string& key);
    void unlockResource(ResourcePtr ptr);

    struct ResourceData {
        std::string key;
        std::chrono::steady_clock::duration maxAge;
        std::chrono::steady_clock::time_point expireTime;
        ResourcePtr resource;
    };

    xec::timer_ptr m_timer;

    // using deque for the stable pointers
    std::deque<ResourceData> m_resources;
};

} // namespace ac::local
