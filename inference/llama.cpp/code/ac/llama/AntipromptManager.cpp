// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AntipromptManager.hpp"

namespace ac::llama {

void AntipromptManager::addAntiprompt(std::string_view antiprompt) {
    m_antiprompts.push_back(std::string(antiprompt));
}

bool AntipromptManager::feedGeneratedText(std::string_view text) {
    for (auto& ap : m_antiprompts) {
        if (ap.feedText(text)) {
            reset();
            return true;
        }
    }

    return false;
}

void AntipromptManager::reset() {
    for (auto& ap : m_antiprompts) {
        ap.reset();
    }
}

void AntipromptManager::clear() {
    m_antiprompts.clear();
}

} // namespace ac::llama
