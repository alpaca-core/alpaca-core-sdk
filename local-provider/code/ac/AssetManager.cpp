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
    itlib::flat_map<int, std::vector<std::unique_ptr<AssetSource>>> m_sources;

    astl::tsumap<AssetInfo> m_assets;

    // these must the last members (first to be destroyed)
    // if there are pending tasks, they will be finalized here and they may access other members
    xec::TaskExecutor m_executor;
    xec::ThreadExecution m_execution;

    auto getAssetInfo(std::string& id) {
        auto f = m_assets.find(id);
        if (f != m_assets.end()) {
            return f;
        }
        for (auto& [_, sources] : m_sources) {
            for (auto& source : sources) {
                auto basicInfo = source->checkAssetSync(id);
                if (!basicInfo) continue;
                auto ret = m_assets.try_emplace(m_assets.end(), id, AssetInfo{
                    .source = source.get(),
                    .size = basicInfo->size,
                    .path = astl::move(basicInfo->path),
                });
                return ret;
            }
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
            cb(id, {.error = "Asset not found"});
        });
    }

    void getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb) {
        m_executor.pushTask([this, movecap(cb, id, progressCb)]() mutable {
            auto f = getAssetInfo(id);
            if (f == m_assets.end()) {
                return cb(id, {.error = "Can't get asset. No source"});
            }
            auto& info = f->second;
            if (info.path) {
                return cb(f->first, info);
            }
            try {
                auto res = info.source->fetchAssetSync(id, [&](float p) {
                    progressCb(id, p);
                });

                info.size = res.size;
                info.path = astl::move(res.path);
            }
            catch (std::exception& ex) {
                info.error = ex.what();
            }
            return cb(f->first, info);
        });
    }

    void addSource(std::unique_ptr<AssetSource> source, int priority) {
        m_executor.pushTask([this, movecap(source), priority]() mutable {
            m_sources[priority].push_back(astl::move(source));
        });
    }
};

AssetManager::AssetManager() : m_impl(std::make_unique<Impl>()) {}
AssetManager::~AssetManager() = default;

void AssetManager::queryAsset(std::string id, QueryAssetCb cb) {
    m_impl->queryAsset(astl::move(id), astl::move(cb));
}

void AssetManager::getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb) {
    m_impl->getAsset(astl::move(id), astl::move(cb), astl::move(progressCb));
}

void AssetManager::addSource(std::unique_ptr<AssetSource> source, int priority) {
    m_impl->addSource(astl::move(source), priority);
}

AssetSource::~AssetSource() = default; // export vtable

}
