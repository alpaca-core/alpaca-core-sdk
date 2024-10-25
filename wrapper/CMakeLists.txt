# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(NOT DEFINED AC_WRAPPER_DEFAULT_VALUE)
    if(alpacaCoreIsRoot)
        set(AC_WRAPPER_DEFAULT_VALUE AUTO)
    else()
        set(AC_WRAPPER_DEFAULT_VALUE OFF)
    endif()
endif()

add_subdirectory(c)
add_subdirectory(java)
add_subdirectory(swift)
add_subdirectory(cocoa)
