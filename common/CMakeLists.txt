# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# public
add_subdirectory(astl)
add_subdirectory(jalogc)

# dev-only
# ...thus only used in certain build flavors

if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES OR AC_BUILD_POC)
    add_subdirectory(ac-repo-root)
    add_subdirectory(ac-test-util)
    add_subdirectory(ac-audio)
endif()

if(AC_BUILD_EXAMPLES)
    # example-only
    add_subdirectory(imgui)
endif()
