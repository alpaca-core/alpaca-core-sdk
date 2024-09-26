// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_C_LOCAL_DUMMY_SHARED
#   if BUILDING_AC_C_LOCAL_DUMMY
#       define AC_C_LOCAL_DUMMY_EXPORT SYMBOL_EXPORT
#   else
#       define AC_C_LOCAL_DUMMY_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_C_LOCAL_DUMMY_EXPORT
#endif

#if defined(__cplusplus)
extern "C" {
#endif

AC_C_LOCAL_DUMMY_EXPORT void ac_add_local_dummy_inference(struct ac_local_model_factory* factory);

#if defined(__cplusplus)
}
#endif
