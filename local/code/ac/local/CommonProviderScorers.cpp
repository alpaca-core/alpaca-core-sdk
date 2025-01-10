// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "CommonProviderScorers.hpp"
#include "Provider.hpp"

namespace ac::local {

ProviderScorer::score_t CanLoadScorer::score(
    const Provider& provider,
    const PluginInfo*,
    const ModelAssetDesc& model,
    const Dict& params
) const noexcept {
    return provider.canLoadModel(model, params);
}

} // namespace ac::local
