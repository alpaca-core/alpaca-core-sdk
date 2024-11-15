# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(dummy_baselib_test test)
    add_doctest_lib_test(${test} aclp-dummy-baselib t-${test}.cpp LIBRARIES ac-test-data::dummy)
endmacro()

dummy_baselib_test(Model)
dummy_baselib_test(Instance)

add_doctest_test(aclp-dummy-plib
    SOURCES
        t-local-dummy-plib.cpp
    LIBRARIES
        aclp::dummy-plib
        ac-test-data::dummy
)

add_doctest_test(aclp-dummy-plugin
    SOURCES
        t-local-dummy-plugin.cpp
    LIBRARIES
        ac::local
        aclp::dummy-info
        ac-test-data::dummy
)
