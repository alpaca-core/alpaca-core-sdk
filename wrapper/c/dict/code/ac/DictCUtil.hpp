// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Dict.hpp>
#include "dict_ref.h"
#include "dict_root.h"
#include "dict_arg.h"

namespace ac::cutil {

/// @defgroup c-cpp-dict C&harr;C++ Dict Helpers
/// Helpers for using the C @ref ac_dict_ref objects in C++ as ac::Dict.
/// @{

/// Convert a C dict ref to a C++ Dict object.
/// The new value is a reference and modifying it will modify the original C dictionary.
AC_C_DICT_EXPORT Dict& Dict_from_dict_ref(ac_dict_ref d);

/// Convert a C++ Dict object to a C dict ref.
/// The new value is a reference and modifying in C it will modify the original C++ dictionary.
AC_C_DICT_EXPORT ac_dict_ref Dict_to_dict_ref(Dict& d);

/// Convert a C dict arg to a C++ Dict object following the arg's intent.
/// - null args become an empty Dict
/// - copy args are deep copied
/// - take args are moved and the original is set to null
AC_C_DICT_EXPORT Dict Dict_from_dict_arg(ac_dict_arg d);

/// @}

} // namespace ac::cutil
