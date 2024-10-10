# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# test only
if(AC_BUILD_TESTS)
    add_subdirectory(ac-test-util)
endif()

# public
add_subdirectory(astl)
add_subdirectory(ahttp)
add_subdirectory(xxhash-cpp)
add_subdirectory(schema)

if(AC_BUILD_TESTS OR AC_BUILD_EXAMPLES OR AC_BUILD_POC)
    add_subdirectory(ac-repo-root)
    add_subdirectory(ac-audio)
    add_subdirectory(ac-test-assets)
endif()

# example-only
if(AC_BUILD_EXAMPLES)
    add_subdirectory(imgui)
endif()
