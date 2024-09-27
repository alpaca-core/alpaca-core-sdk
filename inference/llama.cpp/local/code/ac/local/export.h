// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_LOCAL_LLAMA_SHARED
#   if BUILDING_AC_LOCAL_LLAMA
#       define AC_LOCAL_LLAMA_EXPORT SYMBOL_EXPORT
#   else
#       define AC_LOCAL_LLAMA_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_LOCAL_LLAMA_EXPORT
#endif
