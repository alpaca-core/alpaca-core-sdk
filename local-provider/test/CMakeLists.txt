# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(local_provider_c_test test)
    icm_add_test(
        NAME ac-local-provider-c-${test}
        TARGET test-ac-local-provider-c-${test}
        LIBRARIES
            unity::framework
            ac::api
            ac::local
            ac-dev::test-util
        SOURCES
            ${ARGN}
        FOLDER test
    )
endmacro()

local_provider_c_test(wrapper
    t-wrapper.c
    t-wrapper-dummy-provider.cpp
)

add_doctest_lib_test(inference-provider ac-local
    SOURCES
        t-inference-provider.cpp
    LIBRARIES
        ac::local
        ac::api
)
