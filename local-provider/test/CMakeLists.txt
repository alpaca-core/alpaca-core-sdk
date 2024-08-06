# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(api_c_test test)
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

api_c_test(wrapper
    t-wrapper.c
    t-wrapper-dummy-provider.cpp
)
