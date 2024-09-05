// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct ac_sv ac_sv;

    bool c_to_cxx(ac_sv* a, ac_sv* b);
    bool cxx_to_c(ac_sv* a, ac_sv* b);

#if defined(__cplusplus)
}
#endif
