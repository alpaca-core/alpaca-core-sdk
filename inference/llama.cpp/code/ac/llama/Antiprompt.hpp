// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <vector>
#include <queue>
#include <string_view>

namespace ac::llama {
class AC_LLAMA_EXPORT Antiprompt {
public:
    Antiprompt();
    Antiprompt(std::vector<std::string> antiprompts);

    void addAntiPromptStr(std::string_view antiPrompt);

    void addTokenStr(std::string_view tokenStr);

    bool shouldStop();
private:
    const uint32_t m_maxPrevTokens = 32;
    std::string m_prevTokensStr;
    std::queue<uint16_t> m_tokensSizes;

    std::vector<std::string> m_antiprompts;
};
} // namespace ac::llama
