// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/DictFwd.hpp>
#include <limits>

namespace ac::local {

class ModelLoader;
struct PluginInfo;
struct ModelAssetDesc;

class AC_LOCAL_EXPORT ModelLoaderScorer {
public:
    using score_t = int;

    virtual ~ModelLoaderScorer();

    // ignore the loader if the score is less than or equal to this
    virtual score_t denyScore() const noexcept {
        return std::numeric_limits<score_t>::min();
    }

    // stop the search if the score is greater than or equal to this
    virtual score_t acceptScore() const noexcept {
        return std::numeric_limits<score_t>::max();
    }

    virtual score_t score(
        const ModelLoader& loader,
        const PluginInfo* loaderPlugin,
        const ModelAssetDesc& model,
        const Dict& params
    ) const noexcept = 0;
};

} // namespace ac::local
