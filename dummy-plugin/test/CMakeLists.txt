# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(dummy_baselib_test test)
    add_doctest_lib_test(${test} aclp-dummy-baselib t-${test}.cpp LIBRARIES ac-test-data::dummy)
endmacro()

dummy_baselib_test(Model)
dummy_baselib_test(Instance)

add_doctest_lib_test(i aclp-dummy-plib
    SOURCES
        t-local-dummy-plib.cpp
    LIBRARIES
        ac-test-data::dummy
)
