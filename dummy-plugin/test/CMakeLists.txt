# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(dummy_lib_test test)
    add_doctest_lib_test(${test} aclp-dummy-baselib t-${test}.cpp LIBRARIES ac-test-data::dummy)
endmacro()

dummy_lib_test(Model)
dummy_lib_test(Instance)
