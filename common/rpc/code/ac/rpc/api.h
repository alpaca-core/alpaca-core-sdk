// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_RPC_SHARED
#   if BUILDING_AC_RPC
#       define AC_RPC_API SYMBOL_EXPORT
#   else
#       define AC_RPC_API SYMBOL_IMPORT
#   endif
#else
#   define AC_RPC_API
#endif
