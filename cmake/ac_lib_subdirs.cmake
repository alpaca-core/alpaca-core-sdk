# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# macros which optionally add subdirs according to the project options

macro(ac_add_test_subdir)
    if(AC_BUILD_TESTS)
        add_subdirectory(test)
    endif()
endmacro()

macro(ac_add_example_subdir)
    if(AC_BUILD_EXAMPLES)
        add_subdirectory(example)
    endif()
endmacro()

macro(ac_add_local_subdir)
    # if(AC_BUILD_LOCAL)
    #     add_subdirectory(local)
    # endif()
endmacro()
