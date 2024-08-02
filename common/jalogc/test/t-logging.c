// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <jalogc.h>
#include <ac-test-util/unity.h>
#include <stdio.h>
#include <string.h>

typedef struct entry {
    char scope[20];
    jalogc_log_level level;
    uint64_t timestamp;
    char message[2048];
} entry;

entry entries[1024] = {0};
int entry_count = 0;

void run_cpp_logs(int n);

void log_callback(const char* scope, jalogc_log_level level, uint64_t timestamp, const char* message) {
    if (entry_count >= (int)(sizeof(entries) / sizeof(entries[0]))) {
        TEST_FAIL_MESSAGE("too many entries");
        return;
    }
    entry* e = &entries[entry_count++];
    strncpy(e->scope, scope, sizeof(e->scope));
    e->level = level;
    e->timestamp = timestamp;
    strncpy(e->message, message, sizeof(e->message));
}

void logging(void) {
    jalogc_init_params params = {
        .add_default_sink = true,
        .log_file = stdout,
        .log_callback = log_callback,
    };
    jalogc_init(params);

    for (int i =0; i<3; ++i) {
        jalogc_log(i, "debug message %d", i);
    }

    run_cpp_logs(42);

    CHECK_EQ(5, entry_count);

    uint64_t ts = 0;
    for (int i = 0; i < entry_count; ++i) {
        entry* e = &entries[i];
        CHECK_GTE(ts, e->timestamp);
        ts = e->timestamp;
        ts++;
    }

    entry* e = entries;
    CHECK_EQ_STR("", e->scope);
    CHECK_EQ(JALOGC_LOG_LEVEL_DEBUG, e->level);
    CHECK_EQ_STR("debug message 0", e->message);
    ++e;
    CHECK_EQ_STR("", e->scope);
    CHECK_EQ(JALOGC_LOG_LEVEL_INFO, e->level);
    CHECK_EQ_STR("debug message 1", e->message);
    ++e;
    CHECK_EQ_STR("", e->scope);
    CHECK_EQ(JALOGC_LOG_LEVEL_WARN, e->level);
    CHECK_EQ_STR("debug message 2", e->message);
    ++e;
    CHECK_EQ_STR("", e->scope);
    CHECK_EQ(JALOGC_LOG_LEVEL_ERROR, e->level);
    CHECK_EQ_STR("message from c++ 42", e->message);
    ++e;
    CHECK_EQ_STR("cpp_scope", e->scope);
    CHECK_EQ(JALOGC_LOG_LEVEL_CRIT, e->level);
    CHECK_EQ_STR("scoped from c++ 42", e->message);

    jalogc_shutdown();
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(logging);
    return UNITY_END();
}
