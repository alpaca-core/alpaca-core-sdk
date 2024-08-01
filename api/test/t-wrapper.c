// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/api.h>
#include "s-unity.h"

ac_api_provider* create_dummy_provider(void);
void setUp(void) {}
void tearDown(void) {}

void dummy_provider(void) {
    ac_api_provider* provider = create_dummy_provider();
    CHECK_NOT_NULL(provider);
    ac_free_api_provider(provider);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(dummy_provider);
    return UNITY_END();
}
