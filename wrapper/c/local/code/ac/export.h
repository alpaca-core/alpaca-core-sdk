// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_C_LOCAL_SHARED
#   if BUILDING_AC_C_LOCAL
#       define AC_C_LOCAL_EXPORT SYMBOL_EXPORT
#   else
#       define AC_C_LOCAL_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_C_LOCAL_EXPORT
#endif
