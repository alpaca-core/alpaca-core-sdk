# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(api_test test)
    add_doctest_lib_test(${test} ac-api t-${test}.cpp)
endmacro()

api_test(Dict)

macro(api_c_test test)
    icm_add_test(
        NAME ac-api-c-${test}
        TARGET test-ac-api-c-${test}
        LIBRARIES
            unity::framework
            ac::api
        SOURCES
            t-${test}.c
        FOLDER test
    )
endmacro()

api_c_test(dict)
