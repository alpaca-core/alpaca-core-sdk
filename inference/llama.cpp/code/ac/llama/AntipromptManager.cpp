// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AntipromptManager.hpp"

namespace ac::llama {

AntipromptManager::AntipromptManager(const std::vector<std::string>& antiprompts)
{
    m_antiprompts.reserve(antiprompts.size());
    for (uint32_t i = 0; i < antiprompts.size(); i++) {
        m_antiprompts.push_back(antiprompts[i]);
    }
}

void AntipromptManager::addAntiprompt(std::string_view antiprompt) {
    m_antiprompts.push_back(std::string(antiprompt));
}

bool AntipromptManager::feedGeneratedText(std::string_view text) {
    for (uint32_t i = 0; i < m_antiprompts.size(); i++) {
        if (m_antiprompts[i].feedText(text)) {
            return true;
        }
    }

    return false;
}

void AntipromptManager::reset() {
    m_antiprompts.clear();
}

} // namespace ac::llama
