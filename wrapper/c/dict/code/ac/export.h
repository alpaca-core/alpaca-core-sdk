// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_C_DICT_SHARED
#   if BUILDING_AC_C_DICT
#       define AC_C_DICT_EXPORT SYMBOL_EXPORT
#   else
#       define AC_C_DICT_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_C_DICT_EXPORT
#endif
