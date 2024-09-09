// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>


#if AC_API_SHARED
#   if BUILDING_AC_API
#       define AC_API_EXPORT SYMBOL_EXPORT
#   else
#       define AC_API_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_API_EXPORT
#endif
