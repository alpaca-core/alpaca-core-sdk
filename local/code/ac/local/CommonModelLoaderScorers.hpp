// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ModelLoaderScorer.hpp"

namespace ac::local {

struct AC_LOCAL_EXPORT BooleanScorer : public ModelLoaderScorer {
    score_t denyScore() const noexcept override final { return 0; }
    score_t acceptScore() const noexcept override final { return 1; }
};

struct AC_LOCAL_EXPORT CanLoadScorer final : public BooleanScorer {
    score_t score(
        const ModelLoader& loader,
        const PluginInfo* info,
        const ModelAssetDesc& model,
        const Dict& params
    ) const noexcept override;
};

} // namespace ac::local
