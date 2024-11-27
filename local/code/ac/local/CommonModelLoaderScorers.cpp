// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "CommonModelLoaderScorers.hpp"
#include "ModelLoader.hpp"

namespace ac::local {

ModelLoaderScorer::score_t CanLoadScorer::score(
    const ModelLoader& loader,
    const PluginInfo*,
    const ModelAssetDesc& model,
    const Dict& params
) const noexcept {
    return loader.canLoadModel(model, params);
}

} // namespace ac::local
