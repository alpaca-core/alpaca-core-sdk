# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# macros which optionally add subdirs according to the project options

macro(ac_add_test_subdir)
    if(AC_BUILD_TESTS)
        add_subdirectory(test)
    endif()
endmacro()
