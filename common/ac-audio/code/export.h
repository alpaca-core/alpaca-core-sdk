// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_AUDIO_SHARED
#   if BUILDING_AC_AUDIO
#       define AC_AUDIO_EXPORT SYMBOL_EXPORT
#   else
#       define AC_AUDIO_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_AUDIO_EXPORT
#endif
