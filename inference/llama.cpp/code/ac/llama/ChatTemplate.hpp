// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "ChatMsg.hpp"

namespace ac::llama {
class AC_LLAMA_EXPORT ChatTemplate {
    std::string m_templateId;
public:
    explicit ChatTemplate(std::string templateText);
};
} // namespace ac::llama
}
