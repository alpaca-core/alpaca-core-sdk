// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "IncrementalStringFinder.hpp"

#include <vector>
#include <queue>
#include <string>
#include <string_view>

namespace ac::llama {
class AC_LLAMA_EXPORT AntipromptManager {
public:
    AntipromptManager() = default;

    // adds new antiprompt to check
    void addAntiprompt(std::string_view antiprompt);

    // feed each antiprompt with the text
    bool feedGeneratedText(std::string_view text);

    // reset the state of all antiprompts
    void reset();

    // clear all stored antiprompts
    void clear();
private:
    std::vector<IncrementalStringFinder> m_antiprompts;
};
} // namespace ac::llama
