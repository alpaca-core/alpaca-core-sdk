# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_local_test test)
    add_doctest_lib_test(${test} ac-local t-${test}.cpp ${ARGN})
endmacro()

add_local_test(resource-manager)

configure_file(test-assets.in.h test-assets.h @ONLY)
add_library(ac-local-test-assets INTERFACE)
target_include_directories(ac-local-test-assets INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

add_local_test(FsUtil
    LIBRARIES ac-local-test-assets
)