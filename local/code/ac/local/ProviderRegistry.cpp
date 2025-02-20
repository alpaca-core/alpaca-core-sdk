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

ProviderRegistry::ProviderData ProviderRegistry::findBestProvider(const ProviderScorer& scorer) const noexcept {
    ProviderData best = {};
    auto bestScore = scorer.denyScore();
    auto acceptScore = scorer.acceptScore();

    for (const auto& data : m_providers) {
        auto score = scorer.score(*data.provider, data.plugin);
        if (score > bestScore) {
            best = data;
            bestScore = score;
        }
        if (score >= acceptScore) {
            return best;
        }
    }

    return best;
}

ProviderRegistry::ProviderData ProviderRegistry::findProvider(std::string_view matchName) const noexcept {
    for (const auto& data : m_providers) {
        if (data.provider->info().name.find(matchName) != std::string::npos) {
            return data;
        }
    }
    return {};
}

Provider& ProviderRegistry::getProvider(std::string_view matchName) const {
    auto f = findProvider(matchName);
    if (!f) {
        ac::throw_ex{} << "No provider found for: " << matchName;
    }
    return *f.provider;
}

frameio::SessionHandlerPtr ProviderRegistry::createSessionHandler(std::string_view matchName) {
    for (auto& [provider, _] : m_providers) {
        if (provider->info().name.find(matchName) != std::string::npos) {
            return provider->createSessionHandler({});
        }
    }
    ac::throw_ex{} << "No provider found for: " << matchName;
}

} // namespace ac::local
