# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_rpc_test test)
    add_doctest_lib_test(${test} ac-rpc t-${test}.cpp)
endmacro()
