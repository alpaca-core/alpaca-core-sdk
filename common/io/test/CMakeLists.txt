# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_io_test test)
    add_doctest_lib_test(${test} ac-io t-${test}.cpp)
endmacro()

add_io_test(status)
