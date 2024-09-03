// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

/**
 * @file export.h
 * @brief Defines macros for exporting and importing symbols in the AC API.
 *
 * This file provides macros for marking symbols as exported or imported, which is
 * necessary for building shared libraries on various platforms. It uses the splat
 * library's symbol_export.h for platform-specific implementations.
 */

#if AC_API_SHARED
#   if BUILDING_AC_API
        /**
         * @brief Macro for exporting symbols when building the AC API as a shared library.
         */
#       define AC_API_EXPORT SYMBOL_EXPORT
#   else
        /**
         * @brief Macro for importing symbols when using the AC API as a shared library.
         */
#       define AC_API_EXPORT SYMBOL_IMPORT
#   endif
#else
    /**
     * @brief Macro for symbols when the AC API is not built as a shared library.
     *
     * This macro is defined as empty when the API is not built as a shared library,
     * effectively making the symbols neither exported nor imported.
     */
#   define AC_API_EXPORT
#endif
