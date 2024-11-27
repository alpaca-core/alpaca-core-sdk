// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ModelLoaderRegistry.hpp"
#include "ModelLoader.hpp"
#include "CommonModelLoaderScorers.hpp"
#include "Logging.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <astl/qalgorithm.hpp>
#include <astl/workarounds.h>
#include <charconv>

namespace ac::local {

ModelLoaderRegistry::ModelLoaderRegistry(std::string_view name)
    : m_name(name)
{
    if (m_name.empty()) {
        char hex[20] = "0x";
        auto r = std::to_chars(hex + 2, hex + sizeof(hex), reinterpret_cast<uintptr_t>(this), 16);
        m_name = std::string_view(hex, r.ptr - hex);
    }
}

inline jalog::BasicStream& operator,(jalog::BasicStream& s, const std::vector<std::string>& vec) {
    s, "[";
    for (const auto& v : vec) {
        s, v, ", ";
    }
    s, "]";
    return s;
}

void ModelLoaderRegistry::addLoader(ModelLoader& loader, PluginInfo* plugin) {
    [[maybe_unused]] auto& info = loader.info();
    AC_LOCAL_LOG(Info, "Registry ", m_name, " adding loader ", info.name,
        "\n       vendor: ", info.vendor,
        "\n         tags: ", info.tags
    );

    m_loaders.push_back({&loader, plugin});
}

void ModelLoaderRegistry::removeLoader(ModelLoader& loader) {
    astl::erase_first_if(m_loaders, [&](const auto& data) { return data.loader == &loader; });
}

ModelLoader* ModelLoaderRegistry::findBestLoader(
    const ModelLoaderScorer& scorer, const ModelAssetDesc& desc, const Dict& params
) const {
    ModelLoader* best = nullptr;
    auto bestScore = scorer.denyScore();
    auto acceptScore = scorer.acceptScore();

    for (const auto& data : m_loaders) {
        auto score = scorer.score(*data.loader, data.plugin, desc, params);
        if (score > bestScore) {
            best = data.loader;
            bestScore = score;
        }
        if (score >= acceptScore) {
            return best;
        }
    }

    return best;
}

ModelPtr ModelLoaderRegistry::loadModel(const ModelLoaderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb) const {
    if (auto loader = findBestLoader(scorer, desc, params)) {
        return loader->loadModel(astl::move(desc), astl::move(params), astl::move(cb));
    }

    ac::throw_ex{} << "No loader found for: " << desc.name;
    MSVC_WO_10766806();
}

ModelPtr ModelLoaderRegistry::loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb) const {
    return loadModel(CanLoadScorer{}, astl::move(desc), astl::move(params), astl::move(cb));
}

} // namespace ac::local
