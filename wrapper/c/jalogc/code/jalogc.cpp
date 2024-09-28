// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jalogc.h"

#include <jalog/Instance.hpp>
#include <jalog/Printf.hpp>
#include <jalog/Sink.hpp>
#include <jalog/DefaultScope.hpp>
#include <jalog/sinks/SimpleStdioSink.hpp>
#include <jalog/sinks/DefaultSink.hpp>

#include <cassert>

// these must match as we simply cast one to the other
static_assert(JALOGC_LOG_LEVEL_DEBUG == int(jalog::Level::Debug));
static_assert(JALOGC_LOG_LEVEL_INFO == int(jalog::Level::Info));
static_assert(JALOGC_LOG_LEVEL_WARN == int(jalog::Level::Warning));
static_assert(JALOGC_LOG_LEVEL_ERROR == int(jalog::Level::Error));
static_assert(JALOGC_LOG_LEVEL_CRIT == int(jalog::Level::Critical));
static_assert(JALOGC_LOG_LEVEL_OFF == int(jalog::Level::Off));

namespace  {
jalog::Instance* instance = nullptr;

struct CallbackSink final : public jalog::Sink {
    jalogc_log_callback m_callback;
    CallbackSink(jalogc_log_callback callback) : m_callback(callback) {}
    void record(const jalog::Entry& entry) override {
        m_callback(
            entry.scope.labelCStr(),
            jalogc_log_level(entry.level),
            entry.timestamp.time_since_epoch().count(),
            entry.text.data()
        );
    }
};
} // namespace

extern "C" {

void jalogc_init(jalogc_init_params params) {
    assert(!instance);
    if (instance) {
        jalog::Printf(jalog::Default_Scope, jalog::Level::Error, "Jalog is already initialized");
        return;
    }

    instance = new jalog::Instance();
    auto setup = instance->setup();

    if (params.async_logging) {
        setup.async();
    }

    setup.defaultLevel(jalog::Level(params.default_log_level));

    if (params.add_default_sink) {
        setup.add<jalog::sinks::DefaultSink>();
    }

    if (params.log_file) {
        setup.add<jalog::sinks::SimpleStdioSink>(params.log_file);
    }

    if (params.log_callback) {
        setup.add<CallbackSink>(params.log_callback);
    }
}

void jalogc_shutdown() {
    if (!instance) return; // safe
    delete instance;
    instance = nullptr;
}

void jalogc_log(jalogc_log_level level, _Printf_format_string_ const char* format, ...) {
    auto lvl = jalog::Level(level);
    auto& scope = jalog::Default_Scope;
    if (!scope.enabled(lvl)) return;
    va_list args;
    va_start(args, format);
    VPrintfUnchecked(scope, lvl, format, args);
    va_end(args);
}

}
