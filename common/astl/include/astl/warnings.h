// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once

// macros to enable and disable warnings

#if 0 // example usage
PRAGMA_WARNING_PUSH
DISABLE_MSVC_WARNING(4305)
DISABLE_GCC_AND_CLANG_WARNING("-Wconversion")
double d = 3.14;
float foo1 = d; // no warning
// restore previous warning state
PRAGMA_WARNING_POP
#endif

#if defined(_MSC_VER)
#   define PRAGMA_WARNING_PUSH __pragma(warning(push))
#   define PRAGMA_WARNING_POP  __pragma(warning(pop))
#   define DISABLE_MSVC_WARNING(w) __pragma(warning(disable : w))
#   define DISABLE_GCC_ONLY_WARNING(w)
#   define DISABLE_CLANG_ONLY_WARNING(w)
#   define DISABLE_GCC_AND_CLANG_WARNING(w)
#elif defined(__GNUC__) // also defined by clang
#   define PRAGMA_WARNING_PUSH _Pragma("GCC diagnostic push")
#   define PRAGMA_WARNING_POP  _Pragma("GCC diagnostic pop")
#   define DISABLE_MSVC_WARNING(w)
#   define I_SPLAT_PRAGMA(x) _Pragma(#x)
#   define DISABLE_GCC_AND_CLANG_WARNING(w) I_SPLAT_PRAGMA(GCC diagnostic ignored w)
#   if defined(__clang__)
#       define DISABLE_GCC_ONLY_WARNING(w)
#       define DISABLE_CLANG_ONLY_WARNING(w) I_SPLAT_PRAGMA(GCC diagnostic ignored w)
#   else
#       define DISABLE_GCC_ONLY_WARNING(w) I_SPLAT_PRAGMA(GCC diagnostic ignored w)
#       define DISABLE_CLANG_ONLY_WARNING(w)
#   endif
#else
#   define PRAGMA_WARNING_PUSH
#   define PRAGMA_WARNING_POP
#   define DISABLE_MSVC_WARNING(w)
#   define DISABLE_GCC_ONLY_WARNING(w)
#   define DISABLE_CLANG_ONLY_WARNING(w)
#   define DISABLE_GCC_AND_CLANG_WARNING(w)
#endif
