// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local.h>
#include <ac/local_dummy.h>
#include <ac-unity.h>

void free_null(void) {
    // all must be safe
    ac_free_local_model(NULL);
    ac_free_local_instance(NULL);
    ac_free_local_model_factory(NULL);
}

void setUp(void) {}
void tearDown(void) {}

typedef struct progress_info {
    char tag[1024];
    float progress;
} progress_info;

void on_progress(ac_sv tag, float progress, void* u) {
    if (!u) return;
    progress_info* info = (progress_info*)u;
    size_t len = ac_sv_len(tag);
    CHECK(len < sizeof(info->tag));
    memcpy(info->tag, tag.begin, len);
    info->progress = progress;
}

void no_inference_runner() {
    ac_local_model_factory* factory = ac_new_local_model_factory();
    CHECK_NOT_NULL(factory);
    ac_local_model* model = ac_create_local_model(factory, "nope", NULL, 0, NULL, NULL, NULL);
    CHECK_NULL(model);
    ac_free_local_model_factory(factory);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(free_null);
    RUN_TEST(no_inference_runner);
    //RUN_TEST(dummy_model);
    return UNITY_END();
}