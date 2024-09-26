# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(AC_BUILD_TESTS)
    CPMAddPackage(gh:ThrowTheSwitch/Unity@2.6.0)
    set_target_properties(unity PROPERTIES FOLDER test)
    add_subdirectory(test-util)
endif()

add_subdirectory(cutil)
add_subdirectory(jalogc)

add_subdirectory(dict)
