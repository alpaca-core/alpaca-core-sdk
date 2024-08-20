// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Dict.hpp"
#include "dict_ref.h"
#include "dict_root.h"

// utilities for the c dict api

namespace ac::cutil {
// consume/delete the dict root
// safely return empty dict on null
AC_API_EXPORT Dict Dict_from_dict_root_consume(ac_dict_root* d);

AC_API_EXPORT Dict& Dict_from_dict_ref(ac_dict_ref d);
AC_API_EXPORT ac_dict_ref Dict_to_dict_ref(Dict& d);
AC_API_EXPORT Dict Dict_parse(const char* json, const char* jsonEnd = nullptr);
}
