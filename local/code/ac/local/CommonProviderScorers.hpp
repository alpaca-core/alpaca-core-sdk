// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ProviderScorer.hpp"

namespace ac::local {

struct AC_LOCAL_EXPORT BooleanScorer : public ProviderScorer {
    score_t denyScore() const noexcept override final { return 0; }
    score_t acceptScore() const noexcept override final { return 1; }
};

} // namespace ac::local
