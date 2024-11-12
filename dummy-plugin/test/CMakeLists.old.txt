# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_doctest_lib_test(integration ac-local-dummy
    SOURCES
        t-local-dummy.cpp
    LIBRARIES
        ac-test-data::dummy
        ac::local
)
