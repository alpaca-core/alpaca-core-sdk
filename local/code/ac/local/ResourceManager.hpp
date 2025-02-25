// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"
#include "ResourceQuery.hpp"
#include <ac/xec/strand.hpp>
#include <ac/xec/timer_ptr.hpp>
#include <ac/xec/completion_cb.hpp>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <concepts>
#include <string_view>

namespace ac::xec {
struct simple_coro;
}

namespace ac::local {

class AC_LOCAL_EXPORT ResourceManager {
public:
    ResourceManager(xec::strand ex);
    ~ResourceManager();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    const xec::strand& executor() const noexcept { return m_executor; }

    void queryResource(ResourceQueryPtr query);

private:
    struct ResourceData {
        std::string key;
        std::chrono::steady_clock::time_point expireTime;
        ResourcePtr resource;

        explicit operator bool() const noexcept { return !!resource; }
        void reset() {
            resource.reset();
        }
    };

    xec::strand m_executor;
    xec::timer_ptr m_timer;
    std::vector<ResourceData> m_resources; // sparse

    xec::simple_coro doResourceQuery(ResourceQueryPtr query);
};

} // namespace ac::local
