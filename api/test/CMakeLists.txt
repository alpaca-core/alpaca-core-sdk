# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(api_test test)
    add_doctest_lib_test(${test} ac-api t-${test}.cpp)
endmacro()

api_test(Dict)
