// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// c and c++ compatible zero init

#if defined(__cplusplus)
#   define AC_EMPTY_VAL {}
#   define AC_EMPTY_T(T) {}
#else
#   define AC_EMPTY_VAL {0}
#   define AC_EMPTY_T(T) (T){0}
#endif
