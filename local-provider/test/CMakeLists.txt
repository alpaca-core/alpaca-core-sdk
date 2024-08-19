# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(test-ac-dummy-local-provider STATIC
    t-dummy-local-provider.h
    t-dummy-local-provider.hpp
    t-dummy-local-provider.cpp
)
target_link_libraries(test-ac-dummy-local-provider
    PUBLIC
        ac::local
        ac::api
)

icm_add_test(
    NAME ac-local-provider-c
    TARGET test-ac-local-provider-c
    SOURCES
        t-local-provider.c
    LIBRARIES
        unity::framework
        ac-dev::test-util
        test-ac-dummy-local-provider
    FOLDER test
)

add_doctest_lib_test(provider ac-local
    SOURCES
        t-local-provider.cpp
    LIBRARIES
        test-ac-dummy-local-provider
)
