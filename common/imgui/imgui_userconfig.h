// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if IMGUI_SHARED
#   if BUILDING_IMGUI
#       define IMGUI_API SYMBOL_EXPORT
#   else
#       define IMGUI_API SYMBOL_IMPORT
#   endif
#else
#   define IMGUI_API
#endif

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
// #define IMGUI_DISABLE_DEMO_WINDOWS
