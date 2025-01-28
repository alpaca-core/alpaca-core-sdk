// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <limits>

namespace ac::local {

class Provider;
struct PluginInfo;

class AC_LOCAL_EXPORT ProviderScorer {
public:
    using score_t = int;

    virtual ~ProviderScorer();

    // ignore the provider if the score is less than or equal to this
    virtual score_t denyScore() const noexcept {
        return std::numeric_limits<score_t>::min();
    }

    // stop the search if the score is greater than or equal to this
    virtual score_t acceptScore() const noexcept {
        return std::numeric_limits<score_t>::max();
    }

    virtual score_t score(
        const Provider& provider,
        const PluginInfo* providerPlugin
    ) const noexcept = 0;
};

} // namespace ac::local
