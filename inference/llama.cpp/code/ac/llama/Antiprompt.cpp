// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Antiprompt.hpp"

namespace ac::llama {

Antiprompt::Antiprompt(std::vector<std::string> antiprompts)
    : m_maxAntipromptLength(0)
    , m_antiprompts(std::move(antiprompts))
{
    m_antipromptState.resize(m_antiprompts.size());
    for (uint32_t i = 0; i < m_antiprompts.size(); i++) {
        if (m_maxAntipromptLength < m_antiprompts[i].length()) {
            m_maxAntipromptLength = m_antiprompts[i].length();
        }
    }
}

void Antiprompt::addAntiPromptStr(std::string_view antiprompt) {
    m_antiprompts.push_back(std::string(antiprompt));
    m_antipromptState.push_back({});

    if (m_maxAntipromptLength < antiprompt.length()) {
        m_maxAntipromptLength = antiprompt.length();
    }
}

void Antiprompt::addTokenStr(std::string_view tokenStr) {
    m_prevTokensStr += tokenStr;

    if (m_prevTokensStr.length() > m_maxAntipromptLength) {
        auto lengthDiff = m_prevTokensStr.length() - m_maxAntipromptLength;
        m_prevTokensStr.erase(m_prevTokensStr.begin(), m_prevTokensStr.begin() + lengthDiff);

        for (uint32_t i = 0; i < m_antipromptState.size(); i++) {
            m_antipromptState[i].promptPos = std::max(int32_t(m_antipromptState[i].promptPos - lengthDiff), 0);
        }
    }
}

bool Antiprompt::shouldStop() {
    for (uint32_t i = 0; i < m_antiprompts.size(); i++) {
        // set the start position to search in the end of the sentence,
        // where it's possible to be located the antiprompt.
        // We cannot search only the last token because the antiprompt might be a couple of words
        // if (m_prevTokensStr.find(m_antiprompts[i].c_str(), startPos, m_antiprompts[i].length()) != std::string::npos) {
        //     return true;
        // }
        auto& state = m_antipromptState[i];
        uint32_t promptPos = state.promptPos;
        if (promptPos == 0) {
            promptPos = m_prevTokensStr.length() - m_antiprompts[i].length();
        }

        uint32_t apromptPos = state.antipromptPos;
        for (; promptPos < m_prevTokensStr.length() && apromptPos < m_antiprompts[i].length(); promptPos++, apromptPos++) {
            if (m_antiprompts[i][apromptPos] != m_prevTokensStr[promptPos]) {
                state.antipromptPos = 0;
                state.promptPos = 0;
                break;
            }
        }

        if (apromptPos == m_antiprompts[i].length()) {
            state.antipromptPos = 0;
            state.promptPos = 0;
            return true;
        }
    }

    return false;
}

void Antiprompt::reset() {
    m_prevTokensStr.clear();
    m_antiprompts.clear();
    m_antipromptState.clear();
    m_maxAntipromptLength = 0;
}

} // namespace ac::llama
