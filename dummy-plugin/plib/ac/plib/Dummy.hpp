// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <astl/symbol_export.h>

#if AC_PLIB_DUMMY_SHARED
#   if BUILDING_AC_PLIB_DUMMY
#       define AC_PLIB_DUMMY_EXPORT SYMBOL_EXPORT
#   else
#       define AC_PLIB_DUMMY_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_PLIB_DUMMY_EXPORT
#endif

namespace ac::local {
class ModelFactory;
AC_PLIB_DUMMY_EXPORT void addDummyToModelFactory(local::ModelFactory& factory);
} // namespace ac::local
