// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Init.hpp"
#include "Logging.hpp"
#include <whisper.h>

namespace ac::whisper {

namespace {
static void whisperLogCb(ggml_log_level level, const char* text, void* /*user_data*/) {
    auto len = strlen(text);

    auto jlvl = [&]() {
        switch (level) {
        case GGML_LOG_LEVEL_ERROR: return jalog::Level::Error;
        case GGML_LOG_LEVEL_WARN: return jalog::Level::Warning;
        case GGML_LOG_LEVEL_INFO: return jalog::Level::Info;
        case GGML_LOG_LEVEL_DEBUG: return jalog::Level::Debug;
        default: return jalog::Level::Critical;
        }
    }();

    // skip newlines from llama, as jalog doen't need them
    if (len > 0 && text[len - 1] == '\n') {
        --len;
    }

    log::scope.addEntry(jlvl, {text, len});
}
}


void initLibrary() {
    whisper_log_set(whisperLogCb, nullptr);
    WHISPER_LOG(Info, "cpu info: ", whisper_print_system_info());
}

} // namespace ac::whisper
