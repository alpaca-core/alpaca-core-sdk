# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# test only
if(AC_BUILD_TESTS)
    add_subdirectory(ac-test-util)
endif()

# public
add_subdirectory(astl)
add_subdirectory(jalog)

