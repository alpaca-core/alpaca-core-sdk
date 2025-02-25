// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ResourcePtr.hpp"
#include <concepts>

namespace ac::local {

class ResourceManager;

namespace impl {
class AC_LOCAL_EXPORT BasicResourceLock {
protected:
    BasicResourceLock(ResourceManager& manager) : m_manager(manager) {}
    ~BasicResourceLock() = default;
    ResourceManager& m_manager;
    void release(ResourcePtr&&);
};
} // namespace impl

template <std::derived_from<Resource> R>
class ResourceLock : public impl::BasicResourceLock {
public:
    ResourceLock(ResourceManager& manager, std::shared_ptr<R> resource)
        : m_manager(manager)
        , m_resource(std::move(resource))
    {}
    ResourceLock(ResourceManager& manager, ResourcePtr resource)
        : m_manager(manager)
        , m_resource(std::static_pointer_cast<R>(std::move(resource)))
    {}

    explicit operator bool() const noexcept { return !!m_resource; }

    R* get() const noexcept { return static_cast<R*>(m_resource.get()); }
    R* operator->() const noexcept { return get(); }
    R& operator*() const noexcept { return *get(); }

    ~ResourceLock() {
        release(std::move(m_resource));
    }
private:
    std::shared_ptr<R> m_resource;
};

} // namespace ac::local