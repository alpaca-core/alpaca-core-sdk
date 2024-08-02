// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#if JALOGC_SHARED
#   if BUILDING_JALOGC
#       define JALOGC_EXPORT SYMBOL_EXPORT
#   else
#       define JALOGC_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define JALOGC_EXPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum jalogc_log_level {
    JALOGC_LOG_LEVEL_DEBUG,
    JALOGC_LOG_LEVEL_INFO,
    JALOGC_LOG_LEVEL_WARN,
    JALOGC_LOG_LEVEL_ERROR,
    JALOGC_LOG_LEVEL_CRIT,
    JALOGC_LOG_LEVEL_OFF
} jalogc_log_level;

typedef void (*jalogc_log_callback)(const char* scope, jalogc_log_level level, uint64_t timestamp, const char* message);

typedef struct jalogc_init_params {
    jalogc_log_level default_log_level; // the default log level
    bool add_default_sink; // if true, a default sink is added that logs to stdout and stderr
    FILE* log_file; // optional file to log to
    jalogc_log_callback log_callback; // optional callback for log messages
    bool async_logging; // if true, log messages are queued and processed in a separate thread
} jalogc_init_params;

JALOGC_EXPORT void jalogc_init(jalogc_init_params params);
JALOGC_EXPORT void jalogc_shutdown(); // joins async thread if async_logging is enabled

#if defined(__GNUC__)
#   define I_JALOGC_PRINTF_FMT __attribute__((format(printf, 3, 4)))
#   define _Printf_format_string_
#else
#   define I_JALOGC_PRINTF_FMT
#   if !defined(_MSC_VER)
#       define _Printf_format_string_
#   endif
#endif

JALOGC_EXPORT I_JALOGC_PRINTF_FMT void jalogc_log(jalogc_log_level level, _Printf_format_string_ const char* format, ...);

#if defined(__cplusplus)
}
#endif
