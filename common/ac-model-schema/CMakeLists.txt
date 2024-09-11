# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

ac_dep(nl-json)

add_library(ac-model-schema INTERFACE)
add_library(ac::model-schema ALIAS ac-model-schema)
target_include_directories(ac-model-schema INTERFACE code)

if(AC_BUILD_TESTS)
    add_subdirectory(tests)
endif()

