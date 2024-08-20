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
    explicit AntipromptManager(const std::vector<std::string>& antiprompts);

    // adds new antiprompt to check
    void addAntiprompt(std::string_view antiprompt);

    bool feedGeneratedText(std::string_view text);

    // reset the whole state of object
    void reset();
private:
    std::vector<IncrementalStringFinder> m_antiprompts;
};
} // namespace ac::llama
