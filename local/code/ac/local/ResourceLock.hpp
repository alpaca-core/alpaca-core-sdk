// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"

namespace ac::local {
template <typename R>
class ResourceLock {
public:
    ResourceLock() = default;

    ResourceLock(const ResourceLock&) = default;
    ResourceLock& operator=(const ResourceLock&) = default;
    ResourceLock(ResourceLock&&) noexcept = default;
    ResourceLock& operator=(ResourceLock&&) noexcept = default;

    explicit ResourceLock(std::shared_ptr<R> resource)
        : m_resource(std::move(resource))
    {}

    explicit operator bool() const noexcept { return !!m_resource; }

    R* get() const noexcept { return static_cast<R*>(m_resource.get()); }
    R* operator->() const noexcept { return get(); }
    R& operator*() const noexcept { return *get(); }

    void reset() noexcept {
        if (m_resource) {
            m_resource->touch();
        }
    }

    ~ResourceLock() {
        reset();
    }

    bool operator==(const ResourceLock& other) const noexcept = default;
    auto operator<=>(const ResourceLock& other) const noexcept = default;
private:
    std::shared_ptr<Resource> m_resource;
};

} // namespace ac::local
