# Copyright (c) Borislav Stanimirov
# SPDX-License-Identifier: MIT
#
add_subdirectory(code)
ac_add_test_subdir()
if(AC_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
