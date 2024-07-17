// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ChatMsg.hpp"
#include <span>

struct llama_chat_message;

namespace ac::llama {
class AC_LLAMA_EXPORT ChatFormat {
    std::string m_template;
public:
    // the template string here can be either an id or a markup
    explicit ChatFormat(std::string tpl);

    const std::string& tpl() const noexcept { return m_template; }

    // wrapper around llama_chat_apply_template
    // throw an error on unsupported template
    std::string formatChat(std::span<const ChatMsg> chat, bool addAssistantPrompt) const;

    // format single message taking history into account
    std::string formatMsg(const ChatMsg& msg, std::span<const ChatMsg> history, bool addAssistantPrompt) const;

private:
    std::string apply(std::span<const llama_chat_message> chat, size_t chatSize, bool addAssistantPrompt) const;
};
} // namespace ac::llama
