// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "API.h"

#include "Level.hpp"

#include <cstdarg>

namespace ac::jalog
{

class Scope;

#if defined(__GNUC__)
#   define I_AC_JALOG_PRINTF_FMT __attribute__((format(printf, 3, 4)))
#   define _Printf_format_string_
#else
#   define I_AC_JALOG_PRINTF_FMT
#   if !defined(_MSC_VER)
#       define _Printf_format_string_
#   endif
#endif


AC_JALOG_API void VPrintf(Scope& scope, Level lvl, const char* format, va_list args);
AC_JALOG_API void VPrintfUnchecked(Scope& scope, Level lvl, const char* format, va_list args);

AC_JALOG_API I_AC_JALOG_PRINTF_FMT void Printf(Scope& scope, Level lvl, _Printf_format_string_ const char* format, ...);
AC_JALOG_API I_AC_JALOG_PRINTF_FMT void PrintfUnchecked(Scope& scope, Level lvl, _Printf_format_string_ const char* format, ...);

}
