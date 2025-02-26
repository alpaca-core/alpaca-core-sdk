// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ResourcePtr.hpp"

namespace ac::local {

class ResourceManager;

namespace impl {
AC_LOCAL_EXPORT void Resource_touch(Resource& resource);
} // namespace impl

template <typename R>
class ResourceLock {
public:
    ResourceLock() = default;

    explicit ResourceLock(std::shared_ptr<R> resource)
        : m_resource(std::move(resource))
    {}
    explicit ResourceLock(ResourcePtr resource)
        : m_resource(std::static_pointer_cast<R>(std::move(resource)))
    {}

    explicit operator bool() const noexcept { return !!m_resource; }

    R* get() const noexcept { return static_cast<R*>(m_resource.get()); }
    R* operator->() const noexcept { return get(); }
    R& operator*() const noexcept { return *get(); }

    ~ResourceLock() {
        if (m_resource) {
            impl::Resource_touch(*m_resource);
        }
    }
private:
    std::shared_ptr<R> m_resource;
};

} // namespace ac::local