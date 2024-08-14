// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_WHISPER_SHARED
#   if BUILDING_AC_WHISPER
#       define AC_WHISPER_EXPORT SYMBOL_EXPORT
#   else
#       define AC_WHISPER_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_WHISPER_EXPORT
#endif
