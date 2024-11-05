// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if AC_JALOG_SHARED
#   if BUILDING_AC_JALOG
#       define AC_JALOG_API SYMBOL_EXPORT
#   else
#       define AC_JALOG_API SYMBOL_IMPORT
#   endif
#else
#   define AC_JALOG_API
#endif
