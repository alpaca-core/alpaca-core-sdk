// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jalogc.h"

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/Printf.hpp>
#include <ac/jalog/Sink.hpp>
#include <ac/jalog/DefaultScope.hpp>
#include <ac/jalog/sinks/SimpleStdioSink.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <cassert>

// these must match as we simply cast one to the other
static_assert(JALOGC_LOG_LEVEL_DEBUG == int(ac::jalog::Level::Debug));
static_assert(JALOGC_LOG_LEVEL_INFO == int(ac::jalog::Level::Info));
static_assert(JALOGC_LOG_LEVEL_WARN == int(ac::jalog::Level::Warning));
static_assert(JALOGC_LOG_LEVEL_ERROR == int(ac::jalog::Level::Error));
static_assert(JALOGC_LOG_LEVEL_CRIT == int(ac::jalog::Level::Critical));
static_assert(JALOGC_LOG_LEVEL_OFF == int(ac::jalog::Level::Off));

namespace  {
ac::jalog::Instance* instance = nullptr;

struct CallbackSink final : public ac::jalog::Sink {
    jalogc_log_callback m_callback;
    CallbackSink(jalogc_log_callback callback) : m_callback(callback) {}
    void record(const ac::jalog::Entry& entry) override {
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
        ac::jalog::Printf(ac::jalog::Default_Scope, ac::jalog::Level::Error, "Jalog is already initialized");
        return;
    }

    instance = new ac::jalog::Instance();
    auto setup = instance->setup();

    if (params.async_logging) {
        setup.async();
    }

    setup.defaultLevel(ac::jalog::Level(params.default_log_level));

    if (params.add_default_sink) {
        setup.add<ac::jalog::sinks::DefaultSink>();
    }

    if (params.log_file) {
        setup.add<ac::jalog::sinks::SimpleStdioSink>(params.log_file);
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
    auto lvl = ac::jalog::Level(level);
    auto& scope = ac::jalog::Default_Scope;
    if (!scope.enabled(lvl)) return;
    va_list args;
    va_start(args, format);
    VPrintfUnchecked(scope, lvl, format, args);
    va_end(args);
}

}
