// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if FILE_DOWNLOAD_SHARED
#   if BUILDING_FILE_DOWNLOAD
#       define FILE_DOWNLOAD_EXPORT SYMBOL_EXPORT
#   else
#       define FILE_DOWNLOAD_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define FILE_DOWNLOAD_EXPORT
#endif
