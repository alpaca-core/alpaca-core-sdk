# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(test-ac-dummy-local-provider STATIC
    tl-dummy-local-provider.h
    tl-dummy-local-provider.hpp
    tl-dummy-local-provider.cpp
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
if(MSVC)
    target_compile_options(test-ac-local-provider-c PRIVATE -experimental:c11atomics)
endif()

add_doctest_lib_test(provider ac-local
    SOURCES
        t-local-provider.cpp
    LIBRARIES
        test-ac-dummy-local-provider
        jalog::jalog
        jalog::sinklib
)

macro(local_test test)
    add_doctest_lib_test(${test} ac-local t-${test}.cpp
        LIBRARIES
            ac-dev::test-assets
            ac-dev::test-util
    )
endmacro()

local_test(FsUtil)
local_test(AssetSourceLocalDir)
local_test(AssetSourceLocalFiles)
local_test(AssetManager)
