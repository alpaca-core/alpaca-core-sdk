// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/local_provider.h>

#if defined(__cplusplus)
extern "C" {
#endif

AC_LOCAL_LLAMA_EXPORT void ac_add_local_llama_inference(ac_local_provider* local_provider);

#if defined(__cplusplus)
}
#endif
