// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "IncrementalStringFinder.hpp"

namespace ac::llama {

IncrementalStringFinder::IncrementalStringFinder(std::string searchStr)
    : m_searchStr(std::move(searchStr))
    , m_currentPos(0)
{}

bool IncrementalStringFinder::feedText(std::string_view text) {
    uint32_t promptPos = m_currentPos ? 0 : std::max(int32_t(text.length() - m_searchStr.length()), 0);

    while(promptPos < text.length() && m_currentPos < m_searchStr.length()) {
        if (m_searchStr[m_currentPos] == text[promptPos]) {
            m_currentPos++;
        }
        else {
            // different character was found
            // need to start from the beginning
            m_currentPos = 0;
        }

        promptPos++;
    }

    if (m_currentPos == m_searchStr.length()) {
        m_currentPos = 0;
        return true;
    }

    return false;
}
}
