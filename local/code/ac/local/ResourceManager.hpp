// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Resource.hpp"

#include <astl/qalgorithm.hpp>

#include <memory>
#include <vector>

namespace ac::local {

class ResourceManager {
public:
    // garbage collect expired resources
    // force: disregard expire time
    // return number of resources collected
    int garbageCollect(bool force = false) {
        // if force is true, we set the now to the end of time and now everyone is expired
        auto now = force ? std::chrono::steady_clock::time_point::max() : std::chrono::steady_clock::now();

        int count = 0;
        for (auto& rd : m_resources) {
            if (rd.use_count() != 1) {
                // either empty or shared
                continue;
            }
            if (rd->expireTime() <= now) {
                rd.reset();
                ++count;
            }
        }

        return count;
    }

private:
    template <typename K, typename R>
    friend class ResourceCache;

    void add(const std::shared_ptr<Resource>& resource) {
        // find free slot
        auto p = astl::pfind_if(m_resources, [&](const std::shared_ptr<Resource>& r) { return !r; });

        if (!p) {
            // no free slot was found
            p = &m_resources.emplace_back();
        }

        *p = resource;
    }

    std::vector<std::shared_ptr<Resource>> m_resources;
};

} // namespace ac::local
