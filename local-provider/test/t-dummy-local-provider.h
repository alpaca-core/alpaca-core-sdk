// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/api.h>
#include <stdint.h>

uint64_t get_thread_id(void);
ac_api_provider* create_dummy_provider(void);
void ac_add_local_inference(ac_api_provider* local_provider);
