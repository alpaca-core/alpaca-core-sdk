// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_DUMMY_SHARED
#   if BUILDING_AC_DUMMY
#       define AC_DUMMY_EXPORT SYMBOL_EXPORT
#   else
#       define AC_DUMMY_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_DUMMY_EXPORT
#endif
