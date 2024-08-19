// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Antiprompt.hpp"

namespace ac::llama {

Antiprompt::Antiprompt()
{}

Antiprompt::Antiprompt(std::vector<std::string> antiprompts)
    : m_antiprompts(std::move(antiprompts))
{}

void Antiprompt::addAntiPromptStr(const std::string& antiprompt) {
    m_antiprompts.push_back(antiprompt);
}

void Antiprompt::addTokenStr(const std::string& tokenStr) {
    if (m_tokensSizes.size() == m_maxPrevTokens) {
        m_prevTokensStr.erase(m_prevTokensStr.begin(), m_prevTokensStr.begin() + m_tokensSizes.front());
        m_tokensSizes.pop();
    }

    m_prevTokensStr += tokenStr;
    m_tokensSizes.push(tokenStr.size());
}


bool Antiprompt::shouldStop() {
    for (uint32_t i = 0; i < m_antiprompts.size(); i++)
    {
        // set the start position to search in the end of the sentence,
        // where it's possible to be located the antiprompt.
        // We cannot search only the last token because the anti prompt might be a couple of words
        const auto startPos = m_prevTokensStr.length() - m_antiprompts[i].length();
        if (m_prevTokensStr.find(m_antiprompts[i].c_str(), startPos, m_antiprompts[i].length()) != std::string::npos) {
            return true;
        }
    }

    return false;
}

} // namespace ac::llama
