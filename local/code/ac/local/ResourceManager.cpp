// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ResourceManager.hpp"
#include "ResourceLock.hpp"
#include "Logging.hpp"
#include <ac/xec/timer.hpp>
#include <ac/xec/simple_coro.hpp>
#include <astl/qalgorithm.hpp>

namespace ac::local {

ResourceManager::ResourceManager(xec::strand ex)
    : m_executor(std::move(ex))
    , m_timer(xec::timer::create(m_executor))
{}

ResourceManager::~ResourceManager() = default;

xec::simple_coro ResourceManager::doResourceQuery(ResourceQueryPtr query) {
    {
        auto f = astl::pfind_if(m_resources, [&](const ResourceData& data) {
            return data && data.key == query->key;
        });

        if (f) {
            query->queryCompleteFunc(std::move(f->resource));
            co_return;
        }
    }

    // the resource was not found, so we try to create it
    while (true) {
        auto result = query->tryCreateFunc();
        if (result) {
            // add resource to cache
            auto resource = result.value();
            auto& r = m_resources.emplace_back();
            r.key = std::move(query->key);
            r.expireTime = std::chrono::steady_clock::now() + resource->maxAge;
            r.resource = std::move(resource);

            query->queryCompleteFunc(std::move(result));
            co_return;
        }
        else if (!result.error().noSpace()) {
            // internal error - nothing we can do, propagate and return
            query->queryCompleteFunc(std::move(result));
            co_return;
        }

        // no space, so we need to free some
        query->queryCompleteFunc(std::move(result));
        co_return;
    }
}

void ResourceManager::queryResource(ResourceQueryPtr query) {
    doResourceQuery(std::move(query));
}

} // namespace ac::local
