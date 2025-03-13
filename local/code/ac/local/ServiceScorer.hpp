// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <limits>

namespace ac::local {

struct ServiceInfo;
struct PluginInfo;

class AC_LOCAL_EXPORT ServiceScorer {
public:
    using score_t = int;

    virtual ~ServiceScorer();

    // ignore the service if the score is less than or equal to this
    virtual score_t denyScore() const noexcept {
        return std::numeric_limits<score_t>::min();
    }

    // stop the search if the score is greater than or equal to this
    virtual score_t acceptScore() const noexcept {
        return std::numeric_limits<score_t>::max();
    }

    virtual score_t score(
        const ServiceInfo& serviceInfo,
        const PluginInfo* servicePlugin
    ) const noexcept = 0;
};

} // namespace ac::local
