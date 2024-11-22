# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(dummy_baselib_test test)
    add_doctest_lib_test(${test} aclp-dummy-baselib t-${test}.cpp
        LIBRARIES
            ac-test-data::dummy
            ac-dev::test-util
            ac::jalog
    )
endmacro()

dummy_baselib_test(Model)
dummy_baselib_test(Instance)

add_doctest_test(aclp-dummy-plib
    SOURCES
        t-dummy-plib.cpp
    LIBRARIES
        ac-dev::test-util
        aclp::dummy-plib
        ac-test-data::dummy
        ac::jalog
)

add_doctest_test(aclp-dummy-schema
    SOURCES
        t-dummy-schema.cpp
    LIBRARIES
        ac-dev::test-util
        aclp::dummy-plib
        ac-test-data::dummy
        ac::jalog
)

add_doctest_test(aclp-dummy-plugin
    SOURCES
        t-dummy-plugin.cpp
    LIBRARIES
        ac-dev::test-util
        ac::local
        aclp::dummy-info
        ac-test-data::dummy
        ac::jalog
)
