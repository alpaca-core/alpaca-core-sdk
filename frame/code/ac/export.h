// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_FRAME_SHARED
#   if BUILDING_AC_FRAME
#       define AC_FRAME_EXPORT SYMBOL_EXPORT
#   else
#       define AC_FRAME_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_FRAME_EXPORT
#endif
