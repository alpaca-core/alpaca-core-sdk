// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetManager.hpp"
#include "AssetSource.hpp"

#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>

#include <astl/move_capture.hpp>
#include <astl/move.hpp>
#include <astl/tsumap.hpp>
#include <itlib/flat_map.hpp>

namespace ac {

class AssetManager::Impl {
    // these must the last members (first to be destroyed)
    // if there are pending tasks, they will be finalized here and they may access other members
    xec::TaskExecutor m_executor;
    xec::ThreadExecution m_execution;

    itlib::flat_map<int, std::unique_ptr<AssetSource>> m_sources;

    astl::tsumap<AssetInfo> m_assets;

    auto getAssetInfo(std::string& id) {
        auto f = m_assets.find(id);
        if (f != m_assets.end()) {
            return f;
        }
        for (auto& [_, source] : m_sources) {
            auto basicInfo = source->checkAssetSync(id);
            if (!basicInfo) continue;
            auto ret = m_assets.try_emplace(m_assets.end(), id, AssetInfo{
                .source = source.get(),
                .size = basicInfo->size,
                .path = astl::move(basicInfo->path),
            });
            return ret;
        }
        return m_assets.end();
    }
public:
    Impl() : m_execution(m_executor) {
        m_execution.launchThread("ac-assets");
    }

    void queryAsset(std::string id, QueryAssetCb cb) {
        m_executor.pushTask([this, movecap(cb, id)]() mutable {
            auto f = getAssetInfo(id);
            if (f != m_assets.end()) {
                return cb(f->first, f->second);
            }
            cb(id, { .error = "Asset not found" });
        });
    }

    void getAsset(std::string id, GetAssetCb cb) {
        m_executor.pushTask([this, movecap(cb, id)]() mutable {
            auto f = getAssetInfo(id);
            if (f->second.path) {
                return cb(f->first, f->second);
            }
            auto res = f->second.source->fetchAssetSync(id, {});
            if (res) {
                f->second.size = res->size;
                f->second.path = astl::move(res->path);
            }
            else {
                f->second.error = astl::move(res.error());
            }
            return cb(f->first, f->second);
        });
    }

    void addSource(std::unique_ptr<AssetSource> source, int priority) {
        m_executor.pushTask([this, movecap(source), priority]() mutable {
            m_sources[priority] = astl::move(source);
        });
    }
};

AssetManager::AssetManager() : m_impl(std::make_unique<Impl>()) {}
AssetManager::~AssetManager() = default;

void AssetManager::queryAsset(std::string id, QueryAssetCb cb) {
    m_impl->queryAsset(astl::move(id), astl::move(cb));
}

void AssetManager::getAsset(std::string id, GetAssetCb cb) {
    m_impl->getAsset(astl::move(id), astl::move(cb));
}

void AssetManager::addSource(std::unique_ptr<AssetSource> source, int priority) {
    m_impl->addSource(astl::move(source), priority);
}

AssetSource::~AssetSource() = default; // export vtable

}
