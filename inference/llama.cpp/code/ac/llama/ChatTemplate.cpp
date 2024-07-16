// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ChatTemplate.hpp"
#include <llama.h>
#include <vector>
#include <stdexcept>

namespace ac::llama {
ChatTemplate::ChatTemplate(std::string tpl) : m_template(std::move(tpl)) {}

bool ChatTemplate::verify() const noexcept {
    const llama_chat_message msg = {"user", "test"};
    auto res = llama_chat_apply_template(nullptr, m_template.c_str(), &msg, 1, true, nullptr, 0);
    return res >= 0;
}

namespace {
std::pair<std::vector<llama_chat_message>, size_t> fromChatMsg(std::span<const ChatMsg> chat) {
    std::vector<llama_chat_message> lchat;
    size_t size = 0;
    lchat.reserve(chat.size());
    for (const auto& msg : chat) {
        lchat.push_back({msg.role.c_str(), msg.text.c_str()});
        size += msg.role.size();
        size += msg.text.size();
    }
    return {lchat, size};
}
} // namespace

std::string ChatTemplate::apply(std::span<const ChatMsg> chat, bool addAssistantPrompt) const {
    auto [lchat, size] = fromChatMsg(chat);
    return apply(lchat, size, addAssistantPrompt);
}

std::string ChatTemplate::format(const ChatMsg& msg, std::span<const ChatMsg> history, bool addAssistantPrompt) const {
    auto [lchat, size] = fromChatMsg(history);
    auto fmtHistory = apply(lchat, size, false);

    std::string ret;

    // if the past_msg ends with a newline, we must preserve it in the formatted version
    if (addAssistantPrompt && fmtHistory.ends_with('\n')) {
        ret = "\n";
    };

    lchat.push_back({msg.role.c_str(), msg.text.c_str()});
    size += msg.role.size() + msg.text.size();
    auto fmtNew = apply(lchat, size, addAssistantPrompt);

    // apply diff
    ret += fmtNew.substr(fmtHistory.size());
    return ret;
}

std::string ChatTemplate::apply(std::span<const llama_chat_message> chat, size_t size, bool addAssistantPrompt) const {
    auto allocSize = (size * 5) / 4; // optimistic 25% more than the original size
    std::string fmt(allocSize, '\0');

    // run the first time and get the total output length
    int32_t res = llama_chat_apply_template(nullptr, m_template.c_str(), chat.data(), chat.size(),
        addAssistantPrompt, fmt.data(), int32_t(fmt.size()));

    if (res < 0) {
        throw std::runtime_error("Unsupported template");
    }

    if (size_t(res) > fmt.size()) {
        // optimistic size was not enough
        fmt.resize(res);
        res = llama_chat_apply_template(nullptr, m_template.c_str(), chat.data(), chat.size(),
            addAssistantPrompt, fmt.data(), int32_t(fmt.size()));
        if (res < 0) {
            throw std::runtime_error("WTF! Unsupported template on second try?");
        }
    }

    fmt.resize(res);
    return fmt;
}

} // namespace ac::llama
