# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_subdirectory(lib)

add_subdirectory(plugin)
add_subdirectory(plib)

if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES)
    add_subdirectory(test-models)
endif()

ac_add_test_subdir()
