# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_xec_test test)
    add_doctest_lib_test(${test} ac-xec t-${test}.cpp)
endmacro()

add_xec_test(coro)
