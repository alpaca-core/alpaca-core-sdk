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
    if (m_searchStr.length() == 0) {
        return false;
    }

    uint32_t promptPos = 0;

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

void IncrementalStringFinder::reset() {
    m_currentPos = 0;
}
}
