# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_local_test test)
    add_doctest_lib_test(${test} ac-local t-${test}.cpp)
endmacro()

add_local_test(resource-manager)
