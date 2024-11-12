// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ChatFormat.hpp"
#include <llama.h>
#include <astl/throw_stdex.hpp>
#include <astl/move.hpp>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace ac::llama {
namespace {
bool verify(const std::string& tpl) {
    const llama_chat_message msg = {"user", "test"};
    auto res = llama_chat_apply_template(nullptr, tpl.c_str(), &msg, 1, true, nullptr, 0);
    return res >= 0;
}

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

ChatFormat::ChatFormat(std::string tpl) : m_template(astl::move(tpl)) {
    if (!verify(m_template)) {
        throw_ex{} << "Unsupported template: " << m_template;
    }
}

std::string ChatFormat::formatChat(std::span<const ChatMsg> chat, bool addAssistantPrompt) const {
    auto [lchat, size] = fromChatMsg(chat);
    return apply(lchat, size, addAssistantPrompt);
}

std::string ChatFormat::formatMsg(const ChatMsg& msg, std::span<const ChatMsg> history, bool addAssistantPrompt) const {
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

std::string ChatFormat::apply(std::span<const llama_chat_message> chat, size_t size, bool addAssistantPrompt) const {
    if (size == 0) return {};

    auto allocSize = (size * 5) / 4; // optimistic 25% more than the original size
    std::string fmt(allocSize, '\0');

    // run the first time and get the total output length
    int32_t res = llama_chat_apply_template(nullptr, m_template.c_str(), chat.data(), chat.size(),
        addAssistantPrompt, fmt.data(), int32_t(fmt.size()));

    if (res > int32_t(fmt.size())) {
        // optimistic size was not enough
        fmt.resize(res);
        res = llama_chat_apply_template(nullptr, m_template.c_str(), chat.data(), chat.size(),
            addAssistantPrompt, fmt.data(), int32_t(fmt.size()));
    }

    assert(res >= 0);

    fmt.resize(res);
    return fmt;
}

} // namespace ac::llama
