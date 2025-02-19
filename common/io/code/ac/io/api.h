// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_IO_SHARED
#   if BUILDING_AC_IO
#       define AC_IO_API SYMBOL_EXPORT
#   else
#       define AC_IO_API SYMBOL_IMPORT
#   endif
#else
#   define AC_IO_API
#endif
