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

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(free_null);
    //RUN_TEST(dummy_model);
    return UNITY_END();
}