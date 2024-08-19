// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <vector>
#include <queue>
#include <string>
#include <string_view>

namespace ac::llama {
class AC_LLAMA_EXPORT Antiprompt {
public:
    explicit Antiprompt(std::vector<std::string> antiprompts);

    // adds new antiprompt to check
    void addAntiPromptStr(std::string_view antiPrompt);

    // adds the new string to the checked string
    void addTokenStr(std::string_view tokenStr);

    // checks if the antiprompts can be found in the added tokens so far
    bool shouldStop();

    // reset the whole state of object
    void reset();
private:
    uint32_t m_maxAntipromptLength;
    std::string m_prevTokensStr;

    std::vector<std::string> m_antiprompts;
    struct State {
        uint32_t promptPos = 0;
        uint32_t antipromptPos = 0;
    };
    std::vector<State> m_antipromptState;
};
} // namespace ac::llama
