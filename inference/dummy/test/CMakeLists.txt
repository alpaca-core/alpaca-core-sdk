# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(di_test test)
    add_doctest_lib_test(${test} ac-dummy t-${test}.cpp LIBRARIES ac-test-data::dummy)
endmacro()

di_test(Model)
di_test(Instance)
