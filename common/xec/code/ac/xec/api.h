// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_XEC_SHARED
#   if BUILDING_AC_XEC
#       define AC_XEC_API SYMBOL_EXPORT
#   else
#       define AC_XEC_API SYMBOL_IMPORT
#   endif
#else
#   define AC_XEC_API
#endif
