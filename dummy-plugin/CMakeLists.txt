# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_subdirectory(code)

if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES)
    add_subdirectory(test-models)
endif()

ac_add_test_subdir()
