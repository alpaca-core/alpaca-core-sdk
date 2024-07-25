# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(astl_test test)
    add_doctest_lib_test(${test} astl t-${test}.cpp)
endmacro()

astl_test(iile)
astl_test(mem_ext)
astl_test(throw_ex)
astl_test(u8c)
