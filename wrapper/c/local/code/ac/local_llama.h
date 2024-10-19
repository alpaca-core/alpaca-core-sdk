// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#if defined(__cplusplus)
extern "C" {
#endif

/// Add llama inference to a local model factory.
AC_C_LOCAL_EXPORT void ac_add_local_llama_inference(struct ac_local_model_factory* factory);

#if defined(__cplusplus)
}
#endif
