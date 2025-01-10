// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ProviderRegistry.hpp"
#include "Provider.hpp"
#include "CommonProviderScorers.hpp"
#include "Logging.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <astl/qalgorithm.hpp>
#include <astl/workarounds.h>
#include <charconv>

namespace ac::local {

ProviderRegistry::ProviderRegistry(std::string_view name)
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

void ProviderRegistry::addProvider(Provider& provider, PluginInfo* plugin) {
    [[maybe_unused]] auto& info = provider.info();
    AC_LOCAL_LOG(Info, "Registry ", m_name, " adding provider ", info.name,
        "\n       vendor: ", info.vendor,
        "\n         tags: ", info.tags
    );

    m_providers.push_back({&provider, plugin});
}

void ProviderRegistry::removeProvider(Provider& provider) {
    astl::erase_first_if(m_providers, [&](const auto& data) { return data.provider == &provider; });
}

Provider* ProviderRegistry::findBestProvider(
    const ProviderScorer& scorer, const ModelAssetDesc& desc, const Dict& params
) const {
    Provider* best = nullptr;
    auto bestScore = scorer.denyScore();
    auto acceptScore = scorer.acceptScore();

    for (const auto& data : m_providers) {
        auto score = scorer.score(*data.provider, data.plugin, desc, params);
        if (score > bestScore) {
            best = data.provider;
            bestScore = score;
        }
        if (score >= acceptScore) {
            return best;
        }
    }

    return best;
}

ModelPtr ProviderRegistry::loadModel(const ProviderScorer& scorer, ModelAssetDesc desc, Dict params, ProgressCb cb) const {
    if (auto provider = findBestProvider(scorer, desc, params)) {
        return provider->loadModel(astl::move(desc), astl::move(params), astl::move(cb));
    }

    ac::throw_ex{} << "No provider found for: " << desc.name;
    MSVC_WO_10766806();
}

ModelPtr ProviderRegistry::loadModel(ModelAssetDesc desc, Dict params, ProgressCb cb) const {
    return loadModel(CanLoadScorer{}, astl::move(desc), astl::move(params), astl::move(cb));
}

} // namespace ac::local
