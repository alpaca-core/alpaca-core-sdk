// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_LOCAL_SHARED
#   if BUILDING_AC_LOCAL
#       define AC_LOCAL_EXPORT SYMBOL_EXPORT
#   else
#       define AC_LOCAL_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_LOCAL_EXPORT
#endif
