# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# test only
if(AC_BUILD_TESTS)
    add_subdirectory(ac-test-util)
    add_subdirectory(doctest-unity)
endif()

# public
add_subdirectory(astl)
add_subdirectory(jalogc)
add_subdirectory(ahttp)

# dev-only
# ...thus only used in certain build flavors

if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES OR AC_BUILD_POC)
    add_subdirectory(ac-repo-root)
    add_subdirectory(ac-audio)
endif()

# example-only
if(AC_BUILD_EXAMPLES)
    add_subdirectory(imgui)
endif()
